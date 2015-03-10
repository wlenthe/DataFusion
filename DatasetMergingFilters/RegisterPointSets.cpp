/*
 * Your License or Copyright Information can go here
 */

#include "RegisterPointSets.h"

#include <QtCore/QString>

#include "DatasetMerging/DatasetMergingConstants.h"

#include <Eigen/Dense>
#include <Eigen/SVD>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegisterPointSets::RegisterPointSets() :
  AbstractFilter(),
  m_ReferenceCentroidsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_MovingCentroidsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Centroids),
  m_ReferenceGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_MovingGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_EnsemblePath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, ""),
  m_TransformName("temp"),
  m_ReferenceCentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_MovingCentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_ReferenceGoodFeaturesArrayName(DREAM3D::FeatureData::GoodFeatures),
  m_MovingGoodFeaturesArrayName(DREAM3D::FeatureData::GoodFeatures),
  m_ReferenceCentroids(NULL),
  m_MovingCentroids(NULL),
  m_ReferenceGoodFeatures(NULL),
  m_MovingGoodFeatures(NULL),

  m_AllowTranslation(true),
  m_AllowRotation(true),
  m_AllowScaling(false),
  m_AllowShearing(false),

  m_Transform(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegisterPointSets::~RegisterPointSets()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterPointSets::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Allow Translation", "AllowTranslation", FilterParameterWidgetType::BooleanWidget, getAllowTranslation(), false));
  parameters.push_back(FilterParameter::New("Allow Rotation", "AllowRotation", FilterParameterWidgetType::BooleanWidget, getAllowRotation(), false));
  parameters.push_back(FilterParameter::New("Allow Scaling", "AllowScaling", FilterParameterWidgetType::BooleanWidget, getAllowScaling(), false));
  parameters.push_back(FilterParameter::New("Allow Shearing", "AllowShearing", FilterParameterWidgetType::BooleanWidget, getAllowShearing(), false));
  parameters.push_back(FilterParameter::New("Reference Centroids", "ReferenceCentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceCentroidsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Centroids", "MovingCentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingCentroidsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Good Features", "ReferenceGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceGoodFeaturesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Good Features", "MovingGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingGoodFeaturesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Ensemble Attribute Matrix", "EnsemblePath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getEnsemblePath(), true));
  parameters.push_back(FilterParameter::New("Output Array Name", "TransformName", FilterParameterWidgetType::StringWidget, getTransformName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterPointSets::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  
  setAllowTranslation( reader->readValue("AllowTranslation", getAllowTranslation()));
  setAllowRotation( reader->readValue("AllowRotation", getAllowRotation()));
  setAllowScaling( reader->readValue("AllowScaling", getAllowScaling()));
  setAllowShearing( reader->readValue("AllowShearing", getAllowShearing()));

  setReferenceCentroidsArrayPath( reader->readDataArrayPath( "ReferenceCentroidsArrayPath", getReferenceCentroidsArrayPath() ) );
  setMovingCentroidsArrayPath( reader->readDataArrayPath( "MovingCentroidsArrayPath", getMovingCentroidsArrayPath() ) );
  setReferenceGoodFeaturesArrayPath( reader->readDataArrayPath( "ReferenceGoodFeaturesArrayPath", getReferenceGoodFeaturesArrayPath() ) );
  setMovingGoodFeaturesArrayPath( reader->readDataArrayPath( "MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath() ) );
  setEnsemblePath( reader->readDataArrayPath( "EnsemblePath", getEnsemblePath() ) );
  setTransformName(reader->readString("TransformName", getTransformName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegisterPointSets::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);

  DREAM3D_FILTER_WRITE_PARAMETER(AllowTranslation)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowRotation)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowScaling)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowShearing)


  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(EnsemblePath)
  DREAM3D_FILTER_WRITE_PARAMETER(TransformName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterPointSets::dataCheck()
{
  setErrorCondition(0);

  //get pointers to prereq arrays
  QVector<size_t> dims(1, 3);
  m_ReferenceCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceCentroidsArrayPath(), dims);
  m_MovingCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingCentroidsArrayPath(), dims);

  dims[0] = 1;
  m_ReferenceGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getReferenceGoodFeaturesArrayPath(), dims);
  m_MovingGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMovingGoodFeaturesArrayPath(), dims);

  //assign pointers
  if( NULL != m_ReferenceCentroidsPtr.lock().get() )
  {
    m_ReferenceCentroids = m_ReferenceCentroidsPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingCentroidsPtr.lock().get() )
  {
    m_MovingCentroids = m_MovingCentroidsPtr.lock()->getPointer(0);
  }
  if( NULL != m_ReferenceGoodFeaturesPtr.lock().get() )
  {
    m_ReferenceGoodFeatures = m_ReferenceGoodFeaturesPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingGoodFeaturesPtr.lock().get() )
  {
    m_MovingGoodFeatures = m_MovingGoodFeaturesPtr.lock()->getPointer(0);
  }

  //create transform array
  DataArrayPath tempPath;
  QVector<size_t> transDims(2, 4);
  tempPath.update(getEnsemblePath().getDataContainerName(), getEnsemblePath().getAttributeMatrixName(), getTransformName() );
  m_TransformPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, transDims);
  if( NULL != m_TransformPtr.lock().get() )
  {
    m_Transform = m_TransformPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterPointSets::preflight()
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
const QString RegisterPointSets::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getGroupName()
{
  return "DatasetMerging";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getHumanLabel()
{
  return "Register Centroids";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterPointSets::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  //create list of shared grains with non zero volume
  size_t referenceNumFeatures = m_ReferenceGoodFeaturesPtr.lock()->getNumberOfTuples();
  size_t movingNumFeatures = m_MovingGoodFeaturesPtr.lock()->getNumberOfTuples();
  size_t numFeatures = std::min(referenceNumFeatures, movingNumFeatures);
  std::vector<size_t> sharedFeatures;
  for(size_t i=0; i<numFeatures; i++)
  {
    if(m_ReferenceGoodFeatures[i])
    {
      if(m_MovingGoodFeatures[i])
      {
        sharedFeatures.push_back(i);
      }
    }
  }

  /*
  for each point the affine transform T: {x} = [T]*{xo}:
    x   | T11 T12 T13 T14 |   xo
    y = | T21 T22 T23 T24 | * yo
    z   | T31 T32 T33 T34 |   zo
    w   |  0   0   0   1  |   1

  rewritten:
    x   | xo yo zo  1  0  0  0  0  0  0  0  0 |   | T11 |
    y = |  0  0  0  0 xo yo zo  1  0  0  0  0 | * | T12 |
    z   |  0  0  0  0  0  0  0  0 xo yo zo  1 |   | T13 |
                                                  | T14 |
                                                  | T21 |
                                                  | T22 |
                                                  | T23 |
                                                  | T24 |
                                                  | T31 |
                                                  | T32 |
                                                  | T33 |
                                                  | T34 |

  for the system of points:
    x1   | x1o y1o z1o  1   0   0   0   0   0   0   0   0 |   | T11 |
    y1   |  0   0   0   0  x1o y1o z1o  1   0   0   0   0 |   | T12 |
    z1   |  0   0   0   0   0   0   0   0  x1o y1o z1o  1 |   | T13 |
    x2   | x2o y2o z2o  1   0   0   0   0   0   0   0   0 |   | T14 |
    y2   |  0   0   0   0  x2o y2o z2o  1   0   0   0   0 |   | T21 |
    z2 = |  0   0   0   0   0   0   0   0  x2o y2o z2o  1 | * | T22 |
    x3   | x3o y3o z3o  1   0   0   0   0   0   0   0   0 |   | T23 |
    y3   |  0   0   0   0  x3o y3o z3o  1   0   0   0   0 |   | T24 |
    z3   |  0   0   0   0   0   0   0   0  x3o y3o z3o  1 |   | T31 |
    x4   | x4o y4o z4o  1   0   0   0   0   0   0   0   0 |   | T32 |
    y4   |  0   0   0   0  x4o y4o z4o  1   0   0   0   0 |   | T33 |
    z4   |  0   0   0   0   0   0   0   0  x4o y4o z4o  1 |   | T34 |
    ...  |                       ...                      |

    {b} =                         |A|                        * {x}

   solving linear system for {x} yields affine transfrom coefficients 
  */

  //create/fill A and b matrix/vector
  size_t numPoints = sharedFeatures.size();
  Eigen::MatrixXf A = Eigen::MatrixXf::Zero(3*numPoints, 12);
  Eigen::VectorXf b = Eigen::VectorXf::Zero(3*numPoints);

  for(size_t i=0; i<numPoints; i++)
  {
    size_t index = 3 * i;
    size_t gIndex = 3 * sharedFeatures[i];

    //fill A
    A.row(index+0)<<m_MovingCentroids[gIndex+0], m_MovingCentroids[gIndex+1], m_MovingCentroids[gIndex+2], 1, 0, 0, 0, 0, 0, 0, 0, 0;
    A.row(index+1)<<0, 0, 0, 0, m_MovingCentroids[gIndex+0], m_MovingCentroids[gIndex+1], m_MovingCentroids[gIndex+2], 1, 0, 0, 0, 0;
    A.row(index+2)<<0, 0, 0, 0, 0, 0, 0, 0, m_MovingCentroids[gIndex+0], m_MovingCentroids[gIndex+1], m_MovingCentroids[gIndex+2], 1;

    //fill b
    b(index+0) = m_ReferenceCentroids[gIndex+0];
    b(index+1) = m_ReferenceCentroids[gIndex+1];
    b(index+2) = m_ReferenceCentroids[gIndex+2];
  }

  //solve for affine matrix
  Eigen::VectorXf x = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);

  for(int i=0; i<12; i++)
  {
    m_Transform[i] = x(i);
  }
  m_Transform[12] = 0;
  m_Transform[13] = 0;
  m_Transform[14] = 0;
  m_Transform[15] = 1;


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
AbstractFilter::Pointer RegisterPointSets::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  RegisterPointSets::Pointer filter = RegisterPointSets::New();
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

