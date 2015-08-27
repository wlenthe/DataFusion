/*
 * Your License or Copyright can go here
 */

#include "FuseVolumes.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DynamicTableFilterParameter.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  #include <tbb/parallel_for.h>
  #include <tbb/blocked_range3d.h>
  #include <tbb/partitioner.h>
  #include <tbb/task_scheduler_init.h>
#endif

#include <Eigen/Dense>

#include "DataFusion/DataFusionConstants.h"

#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif

class FuseVolumesImpl
{

  public:
    FuseVolumesImpl(DimType* movingDims, DimType* referenceDims, float* movingOrign, float* referenceOrign, float* movingSpacing, float* referenceSpacing, Eigen::Matrix3f transform, Eigen::Vector3f translation, AttributeMatrix::Pointer movingAttMatt, AttributeMatrix::Pointer fixedAttMatt, QString prefix, int64_t* newIndicies) :
    m_movingDims(movingDims),
    m_movingOrigin(movingOrign),
    m_movingResolution(movingSpacing),
    m_referenceDims(referenceDims),
    m_referenceOrigin(referenceOrign),
    m_referenceResolution(referenceSpacing),
    m_Transfomration(transform),
    m_Translation(translation),
    m_Prefix(prefix),
    m_movingAtrMatPtr(movingAttMatt),
    m_referenceAtrMatPtr(fixedAttMatt),
    m_newIndicies(newIndicies)
    {}
    virtual ~FuseVolumesImpl() {}

    void convert(size_t zStart, size_t zEnd, size_t yStart, size_t yEnd, size_t xStart, size_t xEnd) const
    {
      //vector to hold "good" pairs (ones that map inside the moving dataset)
      std::vector< std::pair<int,int> > indexMap;
      for (size_t k = zStart; k < zEnd; k++)
      {
        int64_t ktot = m_referenceDims[0] * m_referenceDims[1] * k;
        for (size_t j = yStart; j < yEnd; j++)
        {
          int64_t jtot = m_referenceDims[0] * j;
          for (size_t i = xStart; i < xEnd; i++)
          {
            //reference index -> reference position
            Eigen::Vector3f referencePosition;
            referencePosition<< i * m_referenceResolution[0] + m_referenceOrigin[0], j * m_referenceResolution[1] + m_referenceOrigin[1], k * m_referenceResolution[2] + m_referenceOrigin[2];

            //reference position -> moving position
            Eigen::Vector3f movingPosition = m_Transfomration * referencePosition + m_Translation;

            //moving position -> nearest moving index;
            int xIndex = nearbyint( ( movingPosition(0) - m_movingOrigin[0] ) / m_movingResolution[0] );
            int yIndex = nearbyint( ( movingPosition(1) - m_movingOrigin[1] ) / m_movingResolution[1] );
            int zIndex = nearbyint( ( movingPosition(2) - m_movingOrigin[2] ) / m_movingResolution[2] );

            //make sure this is in bounds on the moving dataset
            if( xIndex >= 0 && yIndex >= 0 && zIndex >= 0 && xIndex < m_movingDims[0] && yIndex < m_movingDims[1] && zIndex < m_movingDims[2] )
            {
              //compute indicies + copy
              int movingIndex = zIndex * m_movingDims[1] * m_movingDims[0] + yIndex * m_movingDims[0] + xIndex;
              int fixedIndex = ktot + jtot + i;
              m_newIndicies[fixedIndex] = movingIndex;
            }
            else
            {
              //leave as -1
            }
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range3d<size_t, size_t, size_t>& r) const
    {
      convert(r.pages().begin(), r.pages().end(), r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
    }
#endif

  private:
    DimType* m_movingDims;
    float* m_movingOrigin;
    float* m_movingResolution;
    DimType* m_referenceDims;
    float* m_referenceOrigin;
    float* m_referenceResolution;
    Eigen::Matrix3f m_Transfomration;
    Eigen::Vector3f m_Translation;
    QString m_Prefix;
    AttributeMatrix::Pointer m_movingAtrMatPtr;
    AttributeMatrix::Pointer m_referenceAtrMatPtr;
    int64_t* m_newIndicies;

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FuseVolumes::FuseVolumes() :
  AbstractFilter(),
  m_ReferenceVolume(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_MovingVolume(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_Prefix("fused_"),
  m_TransformationType(0),
  m_TransformationArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DataFusionConstants::Transformation, DataFusionConstants::Transformation)
{
  std::vector<std::vector <double> > identity(3, std::vector<double>(4, 0));
  for(size_t i = 0; i < 3; i++) identity[i][i] = 1;
  DynamicTableData data = getManualTransformation();
  data.setTableData(identity);
  setManualTransformation(data);
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FuseVolumes::~FuseVolumes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FuseVolumes::setupFilterParameters()
{
  FilterParameterVector parameters;
  AttributeMatrixSelectionFilterParameter::DataStructureRequirements amReq;
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Reference Atrribute Matrix", "ReferenceVolume", getReferenceVolume(), FilterParameter::RequiredArray, amReq));
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Moving Atrribute Matrix", "MovingVolume", getMovingVolume(), FilterParameter::RequiredArray, amReq));
  parameters.push_back(StringFilterParameter::New("Merged Array Prefix", "Prefix", getPrefix(), FilterParameter::Parameter));

  {
    QVector<QString> choices;
      choices.push_back("Computed Value");
      choices.push_back("Manual Entry");
    QStringList linkedProps;
      linkedProps << "TransformationArrayPath" << "ManualTransformation";
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
      parameter->setHumanLabel("Transformation Type");
      parameter->setPropertyName("TransformationType");
      parameter->setChoices(choices);
      parameter->setLinkedProperties(linkedProps);
      parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  DataArraySelectionFilterParameter::DataStructureRequirements req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Transformation", "TransformationArrayPath", getTransformationArrayPath(), FilterParameter::RequiredArray, req));

  QStringList headers;
  headers << "" << "" << "" << "";
  parameters.push_back(DynamicTableFilterParameter::New("Transformation", "ManualTransformation", headers, headers, getManualTransformation().getTableData(), FilterParameter::Parameter, false, false, 3, 4));
  parameters.back()->setGroupIndex(1);

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FuseVolumes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceVolume( reader->readDataArrayPath( "ReferenceVolume", getReferenceVolume() ) );
  setMovingVolume( reader->readDataArrayPath( "MovingVolume", getMovingVolume() ) );
  setPrefix( reader->readString("Prefix", getPrefix() ) );
  setTransformationType( reader->readValue("TransformationType", getTransformationType()) );
  setTransformationArrayPath( reader->readDataArrayPath( "TransformationArrayPath", getTransformationArrayPath() ) );
  setManualTransformation(reader->readDynamicTableData("ManualTransformation", getManualTransformation()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FuseVolumes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceVolume)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingVolume)
  DREAM3D_FILTER_WRITE_PARAMETER(Prefix)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformationType)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformationArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ManualTransformation)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FuseVolumes::dataCheck()
{
  setErrorCondition(0);

  //get attribute matricies and ensure they are 3D cell attribute matricies
  AttributeMatrix::Pointer refCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getReferenceVolume(), -303);
  if(getErrorCondition() < 0 || NULL == refCellAttrMat.get() ) { return; }
  if(3 != refCellAttrMat->getTupleDimensions().size() || DREAM3D::AttributeMatrixType::Cell !=refCellAttrMat->getType() )
    notifyErrorMessage(getHumanLabel(), "'Reference Cell Attribute Matrix' must be 3 dimensional rectilinear grid", -1000);

  AttributeMatrix::Pointer moveCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getMovingVolume(), -303);
  if(getErrorCondition() < 0 || NULL == moveCellAttrMat.get() ) { return; }
  if(3 != moveCellAttrMat->getTupleDimensions().size() || DREAM3D::AttributeMatrixType::Cell !=moveCellAttrMat->getType() )
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Attribute Matrix' must be 3 dimensional rectilinear grid", -1000);

  //get geometries and make sure they are image geometeries
  if(DREAM3D::GeometryType::ImageGeometry != getDataContainerArray()->getDataContainer(getReferenceVolume().getDataContainerName())->getGeometry()->getGeometryType())
    notifyErrorMessage(getHumanLabel(), "Rectilinear grid geometry required for Reference Atrribute Matrix.", -390);
  if(DREAM3D::GeometryType::ImageGeometry != getDataContainerArray()->getDataContainer(getMovingVolume().getDataContainerName())->getGeometry()->getGeometryType())
    notifyErrorMessage(getHumanLabel(), "Rectilinear grid geometry required for Moving Atrribute Matrix.", -390);

  //get computed transformation if needed
  if(0 == getTransformationType())
  {
      QVector<size_t> dims(2, 4);
      m_TransformationPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getTransformationArrayPath(), dims);
      if( NULL != m_TransformationPtr.lock().get() )
      { m_Transformation = m_TransformationPtr.lock()->getPointer(0); }
  }

  //loop over attribute arrays of moving, copying to source
  QList<QString> movingArrays = moveCellAttrMat->getAttributeArrayNames();
  size_t numTuples = refCellAttrMat->getNumTuples();
  for(int i = 0; i < movingArrays.size(); i++)
  {
    //create name in reference attr. mat
    QString newName = getPrefix() + movingArrays[i];

    //check for name conflict
    if(refCellAttrMat->doesAttributeArrayExist(newName))
    {
      QString ss = QObject::tr("The selected prefix '%1' creates a name conflict between the 'Reference Cell Attribute Matrix' array '%2' and the 'Moving Cell Attribute Matrix' array '%3'. Please select a different prefix").arg(m_Prefix).arg(newName).arg(movingArrays[i]);
      setErrorCondition(-1001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    //get moving array
    IDataArray::Pointer movingArrPtr = moveCellAttrMat->getAttributeArray(movingArrays[i]);
    if(movingArrPtr.get() != NULL)
    {
      //add equivilent array to reference attr. mat with new name
      IDataArray::Pointer newFixedArray = movingArrPtr->createNewArray(numTuples, movingArrPtr->getComponentDimensions(), newName, true);
      refCellAttrMat->addAttributeArray(newName, newFixedArray);
    }
    else
    {
      //this should never happen since we're getting names from the attribute array
      QString ss = QObject::tr("The array '%1' does not exist in the 'Moving Cell Attribute Matrix; but is expected").arg(movingArrays[i]);
      setErrorCondition(-1002);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  //if the 2 attribute matricies belong to different data containers, also copy all other attribute matricies (so feature + ensemble data carry over)
  if( 0 != getReferenceVolume().getDataContainerName().compare(getMovingVolume().getDataContainerName()) )
  {
    //get both data containers
    DataContainer::Pointer refDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getReferenceVolume().getDataContainerName());
    if(getErrorCondition() < 0 || NULL == refDataContainer ) { return; }

    DataContainer::Pointer moveDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getMovingVolume().getDataContainerName());
    if(getErrorCondition() < 0 || NULL == moveDataContainer ) { return; }

    //loop over moving attribute matricies
    QList<QString> movingAttMatList = moveDataContainer->getAttributeMatrixNames();
    for(int i = 0; i < movingAttMatList.size(); i++)
    {
      //dont copy cell array, its arrays are being copied into an existing attribute matrix
      if( 0 != movingAttMatList[i].compare(getMovingVolume().getAttributeMatrixName()))
      {
        QString newName = getPrefix() + movingAttMatList[i];
        if(refDataContainer->doesAttributeMatrixExist(newName))
        {
          QString ss = QObject::tr("The selected prefix '%1' creates a name conflict between the 'Reference Cell Data Container' attribute matrix '%2' and the 'Moving Cell Data Container' attribute matrix '%3'. Please select a different prefix").arg(m_Prefix).arg(newName).arg(movingAttMatList[i]);
          setErrorCondition(-1002);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }

        AttributeMatrix::Pointer movingAtrMatPtr = moveDataContainer->getAttributeMatrix(movingAttMatList[i]);
        if(movingAtrMatPtr.get() != NULL)
        {
          //add to reference attr. mat with new name
          refDataContainer->addAttributeMatrix(newName, movingAtrMatPtr);
        }
        else
        {
          //this should never happen since we're getting names from the data container
          QString ss = QObject::tr("The attribute matrix '%1' does not exist in the 'Moving Cell Data Container' but is expected").arg(movingAttMatList[i]);
          setErrorCondition(-1003);
          notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
          return;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FuseVolumes::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FuseVolumes::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  //get fixed and moving data containers
  DataContainer::Pointer mReference = getDataContainerArray()->getDataContainer(getReferenceVolume().getDataContainerName());
  DataContainer::Pointer mMoving = getDataContainerArray()->getDataContainer(getMovingVolume().getDataContainerName());

  //get dimensions, origins, and resolutions
  ImageGeom::Pointer refGeom = mReference->getGeometryAs<ImageGeom>();
  ImageGeom::Pointer movGeom = mMoving->getGeometryAs<ImageGeom>();

  //dimensions
  size_t ref_udims[3] = { 0, 0, 0 };
  refGeom->getDimensions(ref_udims);
  DimType refDims[3] = { static_cast<DimType>(ref_udims[0]), static_cast<DimType>(ref_udims[1]), static_cast<DimType>(ref_udims[2]), };

  size_t mov_udims[3] = { 0, 0, 0 };
  movGeom->getDimensions(mov_udims);
  DimType movDims[3] = { static_cast<DimType>(mov_udims[0]), static_cast<DimType>(mov_udims[1]), static_cast<DimType>(mov_udims[2]), };

  //origins
  float refOrigin[3] = {0.0f, 0.0f, 0.0f};
  refGeom->getOrigin(refOrigin);

  float movingOrigin[3] = {0.0f, 0.0f, 0.0f};
  movGeom->getOrigin(movingOrigin);

  //resolutions
  float refRes[3] = {0.0f, 0.0f, 0.0f};
  refGeom->getResolution(refRes);

  float movingRes[3] = {0.0f, 0.0f, 0.0f};
  movGeom->getResolution(movingRes);

  //get attribute matricies
  AttributeMatrix::Pointer refCellAttrMat = mReference->getAttributeMatrix(getReferenceVolume().getAttributeMatrixName());
  AttributeMatrix::Pointer moveCellAttrMat = mMoving->getAttributeMatrix(getMovingVolume().getAttributeMatrixName());

  //fill affine transform and invert
  Eigen::Matrix4f affine;
  if(0 == getTransformationType())
  {
    affine << m_Transformation[0], m_Transformation[1], m_Transformation[2], m_Transformation[3],
              m_Transformation[4], m_Transformation[5], m_Transformation[6], m_Transformation[7],
              m_Transformation[8], m_Transformation[9], m_Transformation[10], m_Transformation[11],
              m_Transformation[12], m_Transformation[13], m_Transformation[14], m_Transformation[15];
  }
  else if(1 == getTransformationType())
  {
    std::vector<std::vector<double> > t = getManualTransformation().getTableData();
    affine << t[0][0], t[0][1], t[0][2], t[0][3],
              t[1][0], t[1][1], t[1][2], t[1][3],
              t[2][0], t[2][1], t[2][2], t[2][3],
              0, 0, 0, 1;
  }
  Eigen::Matrix4f inverseAffine = affine.inverse();

  //split into rotation/shear + translation (to avoid lots of extra multiplying 0*something and 1*1)
  Eigen::Matrix3f transform = inverseAffine.block<3,3>(0,0);
  Eigen::Vector3f translation = inverseAffine.block<3,1>(0,3);

  //compute merged indicies
  size_t numTuples = refDims[0] * refDims[1] * refDims[2];
  DataArray<int64_t>::Pointer newIndiciesPtr = DataArray<int64_t>::CreateArray(numTuples, "New_Indicies");
  newIndiciesPtr->initializeWithValue(-1);
  int64_t* newindicies = newIndiciesPtr->getPointer(0);

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range3d<size_t, size_t, size_t>(0, refDims[2]-1, 0, refDims[1]-1, 0, refDims[0]-1),
                      FuseVolumesImpl(movDims, refDims, movingOrigin, refOrigin, movingRes, refRes, transform, translation, refCellAttrMat, moveCellAttrMat, m_Prefix, newindicies), tbb::auto_partitioner());
  }
  else
#endif
  {
    FuseVolumesImpl serial(movDims, refDims, movingOrigin, refOrigin, movingRes, refRes, transform, translation, refCellAttrMat, moveCellAttrMat, m_Prefix, newindicies);
    serial.convert(0, refDims[2]-1, 0, refDims[1]-1, 0, refDims[0]-1);
  }

  //merge cell attribute matrix
  QList<QString> movingArrayNames = moveCellAttrMat->getAttributeArrayNames();
  for (QList<QString>::iterator iter = movingArrayNames.begin(); iter != movingArrayNames.end(); ++iter)
  {
    //get name of new array in fixed attribute matric
    QString newName = m_Prefix + (*iter);

    //make sure that the destination array actually exists (if source + destination are the same prefix_array has already been added by preflight so this loop will try to copy from prefix_array to prefix_prefix_array
    if(refCellAttrMat->doesAttributeArrayExist(newName))
    {
      IDataArray::Pointer pSourceArray = moveCellAttrMat->getAttributeArray(*iter);
      IDataArray::Pointer pDestArray = refCellAttrMat->getAttributeArray(newName);
      int nComp = pSourceArray->getNumberOfComponents();
      for(int i = 0; i < numTuples; i++)
      {
        if(-1 != newindicies[i])
        {
          void* source = pSourceArray->getVoidPointer(nComp * newindicies[i]);
          void* destination = pDestArray->getVoidPointer(nComp * i);
          ::memcpy(destination, source, pSourceArray->getTypeSize() * nComp);
        }
        else
        {
          pDestArray->initializeTuple(i, 0);
        }
      }
    }
  }

  //copy the remaining att mats if needed (different data containers)
  if( 0 != getReferenceVolume().getDataContainerName().compare(getMovingVolume().getDataContainerName()) )
  {
    //loop over moving attribute matricies
    QList<QString> movingAttMatList = mMoving->getAttributeMatrixNames();
    for(int i = 0; i < movingAttMatList.size(); i++)
    {
      //dont copy cell array, its arrays are have been copied into an existing attribute matrix
      if( 0 != movingAttMatList[i].compare(getMovingVolume().getAttributeMatrixName()))
      {
        //copy attribute matrix from reference to moving
        QString newName = getPrefix() + movingAttMatList[i];
        AttributeMatrix::Pointer movingAtrMatPtr = mMoving->getAttributeMatrix(movingAttMatList[i]);
        mReference->addAttributeMatrix(newName, movingAtrMatPtr);
      }
    }
  }


  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FuseVolumes::newFilterInstance(bool copyFilterParameters)
{
  FuseVolumes::Pointer filter = FuseVolumes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FuseVolumes::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FuseVolumes::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FuseVolumes::getHumanLabel()
{ return "Fuse Volumes"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FuseVolumes::getSubGroupName()
{ return DataFusionConstants::FilterGroups::DataFusionFilters; }

