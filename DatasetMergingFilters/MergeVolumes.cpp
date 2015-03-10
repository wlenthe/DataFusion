/*
 * Your License or Copyright Information can go here
 */

#include "MergeVolumes.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range3d.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QString>

 #include <Eigen/Dense>

#include "DatasetMerging/DatasetMergingConstants.h"

#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif

class MergeVolumesImpl
{

  public:
    MergeVolumesImpl(DimType* movingDims, DimType* referenceDims, float* movingOrign, float* referenceOrign, float* movingSpacing, float* referenceSpacing, Eigen::Matrix3f transform, Eigen::Vector3f translation, AttributeMatrix::Pointer movingAttMatt, AttributeMatrix::Pointer fixedAttMatt, QString prefix, int64_t* newIndicies) :
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
    virtual ~MergeVolumesImpl() {}

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
            int xIndex = std::round( ( movingPosition(0) - m_movingOrigin[0] ) / m_movingResolution[0] );
            int yIndex = std::round( ( movingPosition(1) - m_movingOrigin[1] ) / m_movingResolution[1] );
            int zIndex = std::round( ( movingPosition(2) - m_movingOrigin[2] ) / m_movingResolution[2] );

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
MergeVolumes::MergeVolumes() :
  AbstractFilter(),
  m_ReferenceCellAttributeMatrixArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_MovingCellAttributeMatrixArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, ""),
  m_Prefix("merged_")
{
  m_Row1.a = 1.0f;
  m_Row1.b = 0.0f;
  m_Row1.c = 0.0f;
  m_Row1.d = 0.0f;
  
  m_Row2.a = 0.0f;
  m_Row2.b = 1.0f;
  m_Row2.c = 0.0f;
  m_Row2.d = 0.0f;
  
  m_Row3.a = 0.0f;
  m_Row3.b = 0.0f;
  m_Row3.c = 1.0f;
  m_Row3.d = 0.0f;

  m_Row4.a = 0.0f;
  m_Row4.b = 0.0f;
  m_Row4.c = 0.0f;
  m_Row4.d = 1.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeVolumes::~MergeVolumes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeVolumes::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Reference Cell Attribute Matrix", "ReferenceCellAttributeMatrixArrayPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getReferenceCellAttributeMatrixArrayPath(), false));
  parameters.push_back(FilterParameter::New("Moving Cell Attribute Matrix", "MovingCellAttributeMatrixArrayPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getMovingCellAttributeMatrixArrayPath(), false));
  parameters.push_back(FilterParameter::New("Array Prefix", "Prefix", FilterParameterWidgetType::StringWidget, getPrefix(), true, ""));
  parameters.push_back(FilterParameter::New("Affine Transform", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("", "Row1", FilterParameterWidgetType::FloatVec4Widget, getRow1(), false));
  parameters.push_back(FilterParameter::New("", "Row2", FilterParameterWidgetType::FloatVec4Widget, getRow2(), false));
  parameters.push_back(FilterParameter::New("", "Row3", FilterParameterWidgetType::FloatVec4Widget, getRow3(), false));

  FilterParameter::Pointer param = FilterParameter::New("", "Row4", FilterParameterWidgetType::FloatVec4Widget, getRow4(), false);
  param->setReadOnly(true);
  parameters.push_back(param);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeVolumes::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceCellAttributeMatrixArrayPath( reader->readDataArrayPath("ReferenceCellAttributeMatrixArrayPath", getReferenceCellAttributeMatrixArrayPath() ) );
  setMovingCellAttributeMatrixArrayPath( reader->readDataArrayPath("MovingCellAttributeMatrixArrayPath", getMovingCellAttributeMatrixArrayPath() ) );
  setPrefix( reader->readString("Prefix", getPrefix() ) );
  setRow1( reader->readFloatVec4("Row1", getRow1() ) );
  setRow2( reader->readFloatVec4("Row2", getRow2() ) );
  setRow3( reader->readFloatVec4("Row3", getRow3() ) );
  setRow4( reader->readFloatVec4("Row4", getRow4() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeVolumes::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCellAttributeMatrixArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCellAttributeMatrixArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Prefix)
  DREAM3D_FILTER_WRITE_PARAMETER(Row1)
  DREAM3D_FILTER_WRITE_PARAMETER(Row2)
  DREAM3D_FILTER_WRITE_PARAMETER(Row3)
  DREAM3D_FILTER_WRITE_PARAMETER(Row4)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeVolumes::dataCheck()
{
  setErrorCondition(0);

  if(0 != m_Row4.a || 0 != m_Row4.b || 0 != m_Row4.c || 1 != m_Row4.d )
  {
    notifyErrorMessage(getHumanLabel(), "'Final row of affine transform must be 0 0 0 1", -5);
  }

  //this filter will copy all arrays from 1 attribute matrix to another make sure they show up in pipeline
  AttributeMatrix::Pointer refCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getReferenceCellAttributeMatrixArrayPath(), -303);
  if(getErrorCondition() < 0 || NULL == refCellAttrMat.get() ) { return; }

  AttributeMatrix::Pointer moveCellAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getMovingCellAttributeMatrixArrayPath(), -303);
  if(getErrorCondition() < 0 || NULL == moveCellAttrMat.get() ) { return; }

  //make sure we're transforming between cell
  if(3 != refCellAttrMat->getTupleDimensions().size() || DREAM3D::AttributeMatrixType::Cell !=refCellAttrMat->getType() )
  {
    notifyErrorMessage(getHumanLabel(), "'Reference Cell Attribute Matrix' must be 3 dimensional rectilinear grid", -1000);
  }
  if(3 != moveCellAttrMat->getTupleDimensions().size() || DREAM3D::AttributeMatrixType::Cell !=moveCellAttrMat->getType() )
  {
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Attribute Matrix' must be 3 dimensional rectilinear grid", -1000);
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
  if( 0 != getReferenceCellAttributeMatrixArrayPath().getDataContainerName().compare(getMovingCellAttributeMatrixArrayPath().getDataContainerName()) )
  {
    //get both data containers
    DataContainer::Pointer refDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getReferenceCellAttributeMatrixArrayPath().getDataContainerName());
    if(getErrorCondition() < 0 || NULL == refDataContainer ) { return; }

    DataContainer::Pointer moveDataContainer = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getMovingCellAttributeMatrixArrayPath().getDataContainerName());
    if(getErrorCondition() < 0 || NULL == moveDataContainer ) { return; }

    //loop over moving attribute matricies
    QList<QString> movingAttMatList = moveDataContainer->getAttributeMatrixNames();
    for(int i = 0; i < movingAttMatList.size(); i++)
    {
      //dont copy cell array, its arrays are being copied into an existing attribute matrix
      if( 0 != movingAttMatList[i].compare(getMovingCellAttributeMatrixArrayPath().getAttributeMatrixName()))
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
void MergeVolumes::preflight()
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
const QString MergeVolumes::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeVolumes::getGroupName()
{
  return DatasetMerging::DatasetMergingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeVolumes::getHumanLabel()
{
  return "Merge Volumes";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeVolumes::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::RotationTransformationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeVolumes::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

   //get fixed and moving data containers
  DataContainer::Pointer mReference = getDataContainerArray()->getDataContainer(getReferenceCellAttributeMatrixArrayPath().getDataContainerName());
  DataContainer::Pointer mMoving = getDataContainerArray()->getDataContainer(getMovingCellAttributeMatrixArrayPath().getDataContainerName());

  //make sure that both data containers are rectilinear grids
  if(DREAM3D::GeometryType::ImageGeometry != mReference->getGeometry()->getGeometryType())
  {
    QString ss = QObject::tr("Rectilinear grid geometry required for Reference Cell Attribute Matrix DataContainer.");
    setErrorCondition(-390);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if(DREAM3D::GeometryType::ImageGeometry != mMoving->getGeometry()->getGeometryType())
  {
    QString ss = QObject::tr("Rectilinear grid geometry required for Moving Cell Attribute Matrix DataContainer.");
    setErrorCondition(-390);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //get dimensions, origins, and resolutions
  ImageGeom::Pointer refGeom = mReference->getGeometryAs<ImageGeom>();
  ImageGeom::Pointer movGeom = mMoving->getGeometryAs<ImageGeom>();

  //dimensions
  size_t ref_udims[3] = { 0, 0, 0 };
  refGeom->getDimensions(ref_udims);

  size_t mov_udims[3] = { 0, 0, 0 };
  movGeom->getDimensions(mov_udims);

  DimType refDims[3] = { static_cast<DimType>(ref_udims[0]), static_cast<DimType>(ref_udims[1]), static_cast<DimType>(ref_udims[2]), };
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
  AttributeMatrix::Pointer refCellAttrMat = mReference->getAttributeMatrix(getReferenceCellAttributeMatrixArrayPath().getAttributeMatrixName());
  AttributeMatrix::Pointer moveCellAttrMat = mMoving->getAttributeMatrix(getMovingCellAttributeMatrixArrayPath().getAttributeMatrixName());


  //fill affine transform and invert
  Eigen::Matrix4f affine;
  affine<<m_Row1.a, m_Row1.b, m_Row1.c, m_Row1.d, m_Row2.a, m_Row2.b, m_Row2.c, m_Row2.d, m_Row3.a, m_Row3.b, m_Row3.c, m_Row3.d, m_Row4.a, m_Row4.b, m_Row4.c, m_Row4.d;
  Eigen::Matrix4f inverseAffine = affine.inverse();

  //split into rotation/shear + translation (to avoid lots of extra multiplying 0*something and 1*1)
  Eigen::Matrix3f transform = inverseAffine.block<3,3>(0,0);
  Eigen::Vector3f translation = inverseAffine.block<3,1>(0,3);

  //parallel compute merged indicies
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
                      MergeVolumesImpl(movDims, refDims, movingOrigin, refOrigin, movingRes, refRes, transform, translation, refCellAttrMat, moveCellAttrMat, m_Prefix, newindicies), tbb::auto_partitioner());
  }
  else
#endif
  {
    MergeVolumesImpl serial(movDims, refDims, movingOrigin, refOrigin, movingRes, refRes, transform, translation, refCellAttrMat, moveCellAttrMat, m_Prefix, newindicies);
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
  if( 0 != getReferenceCellAttributeMatrixArrayPath().getDataContainerName().compare(getMovingCellAttributeMatrixArrayPath().getDataContainerName()) )
  {
    //loop over moving attribute matricies
    QList<QString> movingAttMatList = mMoving->getAttributeMatrixNames();
    for(int i = 0; i < movingAttMatList.size(); i++)
    {
      //dont copy cell array, its arrays are have been copied into an existing attribute matrix
      if( 0 != movingAttMatList[i].compare(getMovingCellAttributeMatrixArrayPath().getAttributeMatrixName()))
      {
        //copy attribute matrix from reference to moving
        QString newName = getPrefix() + movingAttMatList[i];
        AttributeMatrix::Pointer movingAtrMatPtr = mMoving->getAttributeMatrix(movingAttMatList[i]);
        mReference->addAttributeMatrix(newName, movingAtrMatPtr);
      }
    }
  }

  if (getCancel() == true)
  {
    /* Gracefully clean up your filter before exiting. */
    return;
  }

  /* If some error occurs this code snippet can report the error up the call chain*/
  if (err < 0)
  {
    QString ss = QObject::tr("Some error message");
    setErrorCondition(-99999999);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MergeVolumes::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  MergeVolumes::Pointer filter = MergeVolumes::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

