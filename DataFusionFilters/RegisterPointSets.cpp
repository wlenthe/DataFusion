/*
 * Your License or Copyright can go here
 */

#include "RegisterPointSets.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/BooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataContainerSelectionFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

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
  m_AttributeMatrixName(DataFusionConstants::Transformation),
  m_TransformName(DataFusionConstants::Transformation),
  m_UseGoodPoints(true),
  m_UseWeights(true),
  m_ReferenceCentroids(NULL),
  m_MovingCentroids(NULL),
  m_ReferenceGoodFeatures(NULL),
  m_MovingGoodFeatures(NULL),
  m_AllowTranslation(true),
  m_AllowRotation(true),
  m_AllowScaling(false),
  m_ScalingType(0),
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

  {
    QStringList linkedProps;
    linkedProps << "ReferenceGoodFeaturesArrayPath" << "MovingGoodFeaturesArrayPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Use Good Points Arrays", "UseGoodPoints", getUseGoodPoints(), linkedProps, FilterParameter::Parameter));
  }
  {
    QStringList linkedProps;
    linkedProps << "WeightsArrayPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Weight Pairs", "UseWeights", getUseWeights(), linkedProps, FilterParameter::Parameter));
  }

  //transform degrees of freedom
  {
    QStringList linkedProps;
    linkedProps << "AllowRotation" << "AllowScaling";
    parameters.push_back(LinkedBooleanFilterParameter::New("Allow Translation", "AllowTranslation", getAllowTranslation(), linkedProps, FilterParameter::Parameter));
  }
  {
    QStringList linkedProps;
    linkedProps << "AllowShearing";
    parameters.push_back(LinkedBooleanFilterParameter::New("Allow Rotation", "AllowRotation", getAllowRotation(), linkedProps, FilterParameter::Parameter));
  }
  {
    QStringList linkedProps;
    linkedProps << "ScalingType" << "AllowShearing";
    parameters.push_back(LinkedBooleanFilterParameter::New("Allow Scaling", "AllowScaling", getAllowScaling(), linkedProps, FilterParameter::Parameter));
  }
  {
    QVector<QString> choices;
      choices.push_back("Isotropic");
      choices.push_back("Anisotropic");
    QStringList linkedProps;
    linkedProps << "AllowShearing";
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
      parameter->setHumanLabel("Scaling Type");
      parameter->setPropertyName("ScalingType");
      parameter->setChoices(choices);
      parameter->setLinkedProperties(linkedProps);
      parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(BooleanFilterParameter::New("Allow Shearing", "AllowShearing", getAllowShearing(), FilterParameter::Parameter, "", 1));
  
  //required arrays
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Points", "ReferenceCentroidsArrayPath", getReferenceCentroidsArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Points", "MovingCentroidsArrayPath", getMovingCentroidsArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Good Points", "ReferenceGoodFeaturesArrayPath", getReferenceGoodFeaturesArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Good Points", "MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Weights", "WeightsArrayPath", getWeightsArrayPath(), FilterParameter::RequiredArray));
  
  //created arrays
  parameters.push_back(StringFilterParameter::New("Output Attribute Matrix Name", "AttributeMatrixName", getAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Output Array Name", "TransformName", getTransformName(), FilterParameter::CreatedArray, 1));
  
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
  setScalingType( reader->readValue("ScalingType", getScalingType()));
  setAllowShearing( reader->readValue("AllowShearing", getAllowShearing()));
  setReferenceCentroidsArrayPath( reader->readDataArrayPath( "ReferenceCentroidsArrayPath", getReferenceCentroidsArrayPath() ) );
  setMovingCentroidsArrayPath( reader->readDataArrayPath( "MovingCentroidsArrayPath", getMovingCentroidsArrayPath() ) );
  setUseGoodPoints( reader->readValue("UseGoodPoints", getUseGoodPoints()));
  setReferenceGoodFeaturesArrayPath( reader->readDataArrayPath( "ReferenceGoodFeaturesArrayPath", getReferenceGoodFeaturesArrayPath() ) );
  setMovingGoodFeaturesArrayPath( reader->readDataArrayPath( "MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath() ) );
  setUseWeights( reader->readValue("UseWeights", getUseWeights()));
  setWeightsArrayPath( reader->readDataArrayPath( "WeightsArrayPath", getWeightsArrayPath() ) );
  setAttributeMatrixName( reader->readString( "AttributeMatrixName", getAttributeMatrixName() ) );
  setTransformName(reader->readString("TransformName", getTransformName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegisterPointSets::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowTranslation)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowRotation)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowScaling)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalingType)
  DREAM3D_FILTER_WRITE_PARAMETER(AllowShearing)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCentroidsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodPoints)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(UseWeights)
  DREAM3D_FILTER_WRITE_PARAMETER(WeightsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixName)
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

  //make sure degrees of freedom are an allowed combination
  if(!getAllowTranslation())
      notifyErrorMessage(getHumanLabel(), "Translation Required", -1);
  if(getAllowShearing() && (!getAllowRotation() || !getAllowScaling() || 1 != getScalingType()) )
      notifyErrorMessage(getHumanLabel(), "Shearing Requires Rotation and Anisotropic Scaling (full affine transformation)", -1);

  //required arrays
  QVector<size_t> dims(1, 3);
  QVector<DataArrayPath> referenceDataArrayPaths, movingDataArrayPaths;

  m_ReferenceCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceCentroidsArrayPath(), dims);
  if( NULL != m_ReferenceCentroidsPtr.lock().get() ) m_ReferenceCentroids = m_ReferenceCentroidsPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getReferenceCentroidsArrayPath());

  m_MovingCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingCentroidsArrayPath(), dims);
  if( NULL != m_MovingCentroidsPtr.lock().get() ) m_MovingCentroids = m_MovingCentroidsPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) movingDataArrayPaths.push_back(getMovingCentroidsArrayPath());

  //centroids must belong to imagegeometery
  if(getErrorCondition() < 0) return;
  if(DREAM3D::GeometryType::ImageGeometry != getDataContainerArray()->getDataContainer(getReferenceCentroidsArrayPath().getDataContainerName())->getGeometry()->getGeometryType())
    notifyErrorMessage(getHumanLabel(), "Rectilinear grid geometry required for Reference Atrribute Matrix.", -390);
  
  if(DREAM3D::GeometryType::ImageGeometry != getDataContainerArray()->getDataContainer(getMovingCentroidsArrayPath().getDataContainerName())->getGeometry()->getGeometryType())
    notifyErrorMessage(getHumanLabel(), "Rectilinear grid geometry required for Moving Atrribute Matrix.", -390);

  dims[0] = 1;
  if(getUseGoodPoints())
  {
    m_ReferenceGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getReferenceGoodFeaturesArrayPath(), dims);
    if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getReferenceGoodFeaturesArrayPath());
    if( NULL != m_ReferenceGoodFeaturesPtr.lock().get() ) m_ReferenceGoodFeatures = m_ReferenceGoodFeaturesPtr.lock()->getPointer(0);
    
    m_MovingGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMovingGoodFeaturesArrayPath(), dims);
    if(getErrorCondition() >= 0) movingDataArrayPaths.push_back(getMovingGoodFeaturesArrayPath());
    if( NULL != m_MovingGoodFeaturesPtr.lock().get() ) m_MovingGoodFeatures = m_MovingGoodFeaturesPtr.lock()->getPointer(0);
  }

  if(getUseWeights())
  {
    m_WeightsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getWeightsArrayPath(), dims);
    if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getWeightsArrayPath());
    if( NULL != m_WeightsPtr.lock().get() ) m_Weights = m_WeightsPtr.lock()->getPointer(0);
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, referenceDataArrayPaths);
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, movingDataArrayPaths);
  if(getErrorCondition() < 0) return;

  //created arrays
    QVector<size_t> tDims(1, 1);//1 spot (single transformation)
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getReferenceCentroidsArrayPath().getDataContainerName());
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::MetaData);
   
  //create array to hold transformation
  DataArrayPath tempPath;
  QVector<size_t> transDims(2, 4);//4x4 array
  tempPath.update(getReferenceCentroidsArrayPath().getDataContainerName(), getAttributeMatrixName(), getTransformName() );
  m_TransformPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, transDims);
  if( NULL != m_TransformPtr.lock().get() ) m_Transform = m_TransformPtr.lock()->getPointer(0);
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
void RegisterPointSets::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  //check degrees of freedom  
  if(!getAllowTranslation())
      notifyErrorMessage(getHumanLabel(), "Translation Required", -1);
  bool affine = false;
  if(getAllowShearing())
  {
    if(getAllowRotation() && getAllowScaling() && 1 == getScalingType())
      affine = true;
    else
      notifyErrorMessage(getHumanLabel(), "Shearing Requires Rotation and Anisotropic Scaling (full affine transformation)", -1);
  }

  //fill transformation with identity matrix by default (in case filter exits early)
  m_Transform[0] = 1;
  m_Transform[1] = 0;
  m_Transform[2] = 0;
  m_Transform[3] = 0;

  m_Transform[4] = 0;
  m_Transform[5] = 1;
  m_Transform[6] = 0;
  m_Transform[7] = 0;

  m_Transform[8] = 0;
  m_Transform[9] = 0;
  m_Transform[10] = 1;
  m_Transform[11] = 0;

  m_Transform[12] = 0;
  m_Transform[13] = 0;
  m_Transform[14] = 0;
  m_Transform[15] = 1;

  //create list of matched points
  size_t referenceNumFeatures = m_ReferenceCentroidsPtr.lock()->getNumberOfTuples();
  size_t movingNumFeatures = m_MovingCentroidsPtr.lock()->getNumberOfTuples();
  size_t numFeatures = std::min(referenceNumFeatures, movingNumFeatures);
  std::vector<size_t> sharedFeatures;
  sharedFeatures.reserve(numFeatures);
  
  for(size_t i = 0; i < numFeatures; i++)
  {
    //dont add bad points to list
    if(getUseGoodPoints())
    {
      if( !(m_ReferenceGoodFeatures[i] && m_MovingGoodFeatures[i]) )
        continue;
    }
    sharedFeatures.push_back(i);
  }
  numFeatures = sharedFeatures.size();

  //create matrix + vector to hold transfomration + translation (default to identiy matrix and no translation)
  Eigen::Matrix3d transformation = Eigen::Matrix3d::Identity();
  Eigen::Vector3d translation = Eigen::Vector3d::Zero();

  //compute center of mass for each point set (computations require both sets centered at origin)
  Eigen::Vector3d xBar = Eigen::Vector3d::Zero();//moving points centroid
  Eigen::Vector3d yBar = Eigen::Vector3d::Zero();//reference points centroid
  float totalW = 0.0f;
  for(size_t i = 0; i < numFeatures; i++)
  {
    size_t index = 3 * sharedFeatures[i];
    float w = 1.0f;
    if(getUseWeights())
      w = m_Weights[sharedFeatures[i]];
    xBar += w * Eigen::Vector3d(m_MovingCentroids[index + 0], m_MovingCentroids[index + 1], m_MovingCentroids[index + 2]);
    yBar += w * Eigen::Vector3d(m_ReferenceCentroids[index + 0], m_ReferenceCentroids[index + 1], m_ReferenceCentroids[index + 2]);
  }

  xBar /= numFeatures;
  yBar /= numFeatures;
  if(getUseWeights())
  {
    xBar /= totalW;
    yBar /= totalW;
  }

  //compute variance and covariance matrices
  Eigen::Matrix3d covariance = Eigen::Matrix3d::Zero();
  Eigen::Matrix3d variance = Eigen::Matrix3d::Zero();
  for(size_t i = 0; i < numFeatures; i++)
  {
    size_t index = 3 * sharedFeatures[i];
    Eigen::Vector3d x(m_MovingCentroids[index + 0], m_MovingCentroids[index + 1], m_MovingCentroids[index + 2]);
    Eigen::Vector3d y(m_ReferenceCentroids[index + 0], m_ReferenceCentroids[index + 1], m_ReferenceCentroids[index + 2]);
    x -= xBar;
    y -= yBar;
    float w = 1.0f;
    if(getUseWeights())
      w = m_Weights[sharedFeatures[i]];
    covariance += w * Eigen::Matrix3d(x * y.transpose());
    variance += w * Eigen::Matrix3d(x * x.transpose());
  }

  //shear (requires full affine) and no shear are handled differently
  if(affine)
  {
    bool invertible;
    Eigen::Matrix3d inverse;
    variance.computeInverseWithCheck(inverse, invertible);
    if(invertible)
      transformation = inverse * covariance;
    else
      notifyWarningMessage(getHumanLabel(), "singular variance matrix", 1);
    transformation.transposeInPlace();
  }
  else
  {
    if(getAllowRotation())
    {
      //perform singular value decomposition of covariance matrix
      Eigen::JacobiSVD<Eigen::Matrix3d> svd(covariance, Eigen::ComputeFullU | Eigen::ComputeFullV);
      
      //check determinates (det(V) * det(U) == 1 for rotation or -1 for rotation + reflection)
      //least square rotation matrix is V * I * U^T if det(V) == 1, V * {{1,0,0},{0,1,0},{0,0,-1}} * U^T if == -1
      Eigen::Matrix3d i = Eigen::Matrix3d::Identity();
      if(svd.matrixV().determinant() * svd.matrixU().determinant() < 0)
        i(2, 2) = -1;
      transformation = svd.matrixV() * i * svd.matrixU().transpose();
    }

    if(getAllowScaling())
    {
      Eigen::Matrix3d scale = Eigen::Matrix3d::Identity();
      Eigen::Matrix3d numerator = transformation * covariance;
      Eigen::Matrix3d denomenator = transformation * variance * transformation.transpose();

      if(0 == getScalingType())//isotropic scaling
        scale.diagonal().fill( numerator.trace() / denomenator.trace() );
      else//anisotropic scaling
        scale.diagonal() = numerator.diagonal().cwiseQuotient(denomenator.diagonal());

      transformation = transformation * scale;
    }
  }

  //centroids are absolulte but transormation must be w.r.t. origin
  float refOrigin[3] = {0.0f, 0.0f, 0.0f};
  float movOrigin[3] = {0.0f, 0.0f, 0.0f};

  getDataContainerArray()->getDataContainer(getReferenceCentroidsArrayPath().getDataContainerName())->getGeometryAs<ImageGeom>()->getOrigin(refOrigin);
  getDataContainerArray()->getDataContainer(getMovingCentroidsArrayPath().getDataContainerName())->getGeometryAs<ImageGeom>()->getOrigin(movOrigin);

  yBar += Eigen::Vector3d(refOrigin[0], refOrigin[1], refOrigin[2]);
  xBar += Eigen::Vector3d(movOrigin[0], movOrigin[1], movOrigin[2]);

  //compute translation and fill in transformation
  translation = yBar - transformation * xBar;

  m_Transform[0] = transformation(0, 0);
  m_Transform[1] = transformation(0, 1);
  m_Transform[2] = transformation(0, 2);
  m_Transform[3] = translation(0);
  
  m_Transform[4] = transformation(1, 0);
  m_Transform[5] = transformation(1, 1);
  m_Transform[6] = transformation(1, 2);
  m_Transform[7] = translation(1);

  m_Transform[8] = transformation(2, 0);
  m_Transform[9] = transformation(2, 1);
  m_Transform[10] = transformation(2, 2);
  m_Transform[11] = translation(2);

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegisterPointSets::newFilterInstance(bool copyFilterParameters)
{
  RegisterPointSets::Pointer filter = RegisterPointSets::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getHumanLabel()
{ return "Register Point Sets"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getSubGroupName()
{ return "DataFusion"; }

