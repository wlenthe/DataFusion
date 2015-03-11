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
  m_AttributeMatrixName(DatasetMerging::Transformation),
  m_TransformName(DatasetMerging::Transformation),
  m_ReferenceCentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_MovingCentroidsArrayName(DREAM3D::FeatureData::Centroids),
  m_UseGoodPoints(true),
  m_ReferenceGoodFeaturesArrayName(DREAM3D::FeatureData::GoodFeatures),
  m_MovingGoodFeaturesArrayName(DREAM3D::FeatureData::GoodFeatures),
  m_UseWeights(true),
  m_WeightsArrayName(DREAM3D::FeatureData::Volumes),
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

  parameters.push_back(FilterParameter::New("Transformation Degrees of Freedom", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Allow Translation", "AllowTranslation", FilterParameterWidgetType::BooleanWidget, getAllowTranslation(), false));
  parameters.push_back(FilterParameter::New("Allow Rotation", "AllowRotation", FilterParameterWidgetType::BooleanWidget, getAllowRotation(), false));
  {
    QStringList linkedProps;
    linkedProps << "ScalingType";
    parameters.push_back(LinkedBooleanFilterParameter::New("Allow Scaling", "AllowScaling", getAllowScaling(), linkedProps, false));
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scaling Type");
    parameter->setPropertyName("ScalingType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("Isotropic");
    choices.push_back("Anisotropic");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Allow Shearing", "AllowShearing", FilterParameterWidgetType::BooleanWidget, getAllowShearing(), false));
  
  parameters.push_back(FilterParameter::New("Point Set Selection", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Reference Points", "ReferenceCentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceCentroidsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Points", "MovingCentroidsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingCentroidsArrayPath(), false, ""));
  {
    QStringList linkedProps;
    linkedProps << "ReferenceGoodFeaturesArrayPath" << "MovingGoodFeaturesArrayPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Use Good Points Arrays", "UseGoodPoints", getUseGoodPoints(), linkedProps, false));
  }
  parameters.push_back(FilterParameter::New("Reference Good Points", "ReferenceGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceGoodFeaturesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Good Points", "MovingGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingGoodFeaturesArrayPath(), false, ""));

  {
    QStringList linkedProps;
    linkedProps << "WeightsArrayPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Weight Pairs", "UseWeights", getUseWeights(), linkedProps, false));
  }
  parameters.push_back(FilterParameter::New("Weights", "WeightsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getWeightsArrayPath(), false, ""));
  

  parameters.push_back(FilterParameter::New("Output Attribute Matrix Name", "AttributeMatrixName", FilterParameterWidgetType::StringWidget, getAttributeMatrixName(), true, ""));
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
  if(getAllowRotation())
  {
    if(!getAllowTranslation())
    {
      QString ss = QObject::tr("Rotation Requires Translation");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
    }
  }

  if(getAllowShearing())
  {
    if(!getAllowRotation())
    {
      QString ss = QObject::tr("Shearing Requires Rotation");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
    }

    if(!getAllowScaling() || 0 == getScalingType())
    {
      QString ss = QObject::tr("Shearing Requires Anisotropic Scaling");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
    }
  }
  if(getErrorCondition() < 0) {return;}

  //get pointers to prereq arrays
  QVector<size_t> dims(1, 3);
  m_ReferenceCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceCentroidsArrayPath(), dims);
  m_MovingCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingCentroidsArrayPath(), dims);

  if(getUseGoodPoints())
  {
    dims[0] = 1;
    m_ReferenceGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getReferenceGoodFeaturesArrayPath(), dims);
    m_MovingGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMovingGoodFeaturesArrayPath(), dims);
  }
  if(getUseWeights())
  {
    dims[0] = 1;
    m_WeightsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getWeightsArrayPath(), dims);
  }
  if(getErrorCondition() < 0) {return;}

  //assign pointers
  if( NULL != m_ReferenceCentroidsPtr.lock().get() )
  {
    m_ReferenceCentroids = m_ReferenceCentroidsPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingCentroidsPtr.lock().get() )
  {
    m_MovingCentroids = m_MovingCentroidsPtr.lock()->getPointer(0);
  }

  if(getUseGoodPoints())
  {
    if( NULL != m_ReferenceGoodFeaturesPtr.lock().get() )
    {
      m_ReferenceGoodFeatures = m_ReferenceGoodFeaturesPtr.lock()->getPointer(0);
    }
    if( NULL != m_MovingGoodFeaturesPtr.lock().get() )
    {
      m_MovingGoodFeatures = m_MovingGoodFeaturesPtr.lock()->getPointer(0);
    }
  }

  if(getUseWeights())
  {
    if( NULL != m_WeightsPtr.lock().get() )
    {
      m_Weights = m_WeightsPtr.lock()->getPointer(0);
    }
  }

  //create metadata attribute matrix in reference data container to hold transformation
  QVector<size_t> tDims(1, 1);//1 spot (single transformation)
  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getReferenceCentroidsArrayPath().getDataContainerName());
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::MetaData);
   
  //create array to hold transformation
  DataArrayPath tempPath;
  QVector<size_t> transDims(2, 4);//4x4 array
  tempPath.update(getReferenceCentroidsArrayPath().getDataContainerName(), getAttributeMatrixName(), getTransformName() );
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
  return DatasetMerging::DatasetMergingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getHumanLabel()
{
  return "Register Point Sets";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterPointSets::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::AlignmentFilters;
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

  //create list of matched point
  size_t referenceNumFeatures = m_ReferenceCentroidsPtr.lock()->getNumberOfTuples();
  size_t movingNumFeatures = m_MovingCentroidsPtr.lock()->getNumberOfTuples();
  size_t numFeatures = std::min(referenceNumFeatures, movingNumFeatures);
  std::vector<size_t> sharedFeatures;
  for(size_t i = 0; i < numFeatures; i++)
  {
    //dont add bad points to list
    if(getUseGoodPoints())
    {
      if( !(m_ReferenceGoodFeatures[i] && m_MovingGoodFeatures[i]) )
      {
        continue;
      }
    }
    sharedFeatures.push_back(i);
  }
  size_t numPoints = sharedFeatures.size();

  //behavoir depends on degrees of freedom (t=translation, r=rotation, i=isotropic scaling, a=anisotropic scaling, s=shear)
  //I'll allow the following combinations (in ~increasing degrees of freedom)
  //t, ti, ta, tr, tir, tar, tars (full affine)
  //other possible combinations are possible (eg ts or r) but are either not useful or I don't know how to compute the least squared solution
  //weighting of points is possible but has been omitted for now

  //create matrix + vector to hold transfomration + translation (default to identiy matrix and no translation)
  Eigen::Matrix3f transformation = Eigen::Matrix3f::Identity();
  Eigen::Vector3f translation = Eigen::Vector3f::Zero();

  //compute center of mass for each point set (computations require both sets centered at origin)
  Eigen::Vector3f xBar = Eigen::Vector3f::Zero();//moving points centroid
  Eigen::Vector3f yBar = Eigen::Vector3f::Zero();//reference points centroid
  float totalW = 0.0f;
  for(size_t i = 0; i < numPoints; i++)
  {
    size_t index = 3 * sharedFeatures[i];
    float w = 1.0f;
    if(getUseWeights())
    {
      w = m_Weights[sharedFeatures[i]];
    }
    xBar += w * Eigen::Vector3f(m_MovingCentroids[index + 0], m_MovingCentroids[index + 1], m_MovingCentroids[index + 2]);
    yBar += w * Eigen::Vector3f(m_ReferenceCentroids[index + 0], m_ReferenceCentroids[index + 1], m_ReferenceCentroids[index + 2]);
  }
  xBar /= numPoints;
  yBar /= numPoints;
  if(getUseWeights())
  {
    xBar /= totalW;
    yBar /= totalW;
  }

  //compute variance and covariance matrices
  Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();
  Eigen::Matrix3f variance = Eigen::Matrix3f::Zero();
  for(size_t i = 0; i < numPoints; i++)
  {
    size_t index = 3 * sharedFeatures[i];
    Eigen::Vector3f x(m_MovingCentroids[index + 0], m_MovingCentroids[index + 1], m_MovingCentroids[index + 2]);
    Eigen::Vector3f y(m_ReferenceCentroids[index + 0], m_ReferenceCentroids[index + 1], m_ReferenceCentroids[index + 2]);
    x -= xBar;
    y -= yBar;
    float w = 1.0f;
    if(getUseWeights())
    {
      w = m_Weights[sharedFeatures[i]];
    }
    covariance += w * Eigen::Matrix3f(x * y.transpose());
    variance += w * Eigen::Matrix3f(x * x.transpose());
  }

  //shear and no shear are handled differently
  if(getAllowShearing())
  {
    if(getAllowRotation() && getAllowScaling() && 1 == getScalingType())
    {
      //full affine
      transformation = variance.inverse() * covariance;
      transformation.transposeInPlace();
    }
    else
    {
      //bad combination
      QString ss = QObject::tr("Unallowed combination of degree(s) of freedom");
      setErrorCondition(-1);
      notifyErrorMessage(getHumanLabel(), ss, -1);
      return;
    }
  }
  else//no shear
  {
    //compute rotation if allowed
    if(getAllowRotation())
    {
      //perform singular value decomposition of covariance matrix
      Eigen::JacobiSVD<Eigen::Matrix3f> svd(covariance, Eigen::ComputeFullU | Eigen::ComputeFullV);
      
      //check determinates (det(V) * det(U) == 1 for rotation or -1 for rotation + reflection)
      //least square rotation matrix is V * I * U^T if det(V) == 1, V * {{1,0,0},{0,1,0},{0,0,-1}} * U^T if == -1
      Eigen::Matrix3f i = Eigen::Matrix3f::Identity();
      if(svd.matrixV().determinant() * svd.matrixU().determinant() < 0)
      {
        i(2, 2) = -1;
      }
      transformation = svd.matrixV() * i * svd.matrixU().transpose();
    }

    //compute scaling if needed
    if(getAllowScaling())
    {
      Eigen::Matrix3f scale = Eigen::Matrix3f::Identity();
      Eigen::Matrix3f numerator = transformation * covariance;
      Eigen::Matrix3f denomenator = transformation * variance * transformation.transpose();

      if(0 == getScalingType())
      {
        //isotropic scaling
        scale.diagonal().fill( numerator.trace() / denomenator.trace() );
      }
      else//(1 == getScalingType())
      {
        scale.diagonal() = numerator.diagonal().cwiseQuotient(denomenator.diagonal());
      }

      //combine rotation + scaling
      transformation = transformation * scale;
    }
  }

  //compute translation
  if(getAllowTranslation())
  {
    translation = yBar - transformation * xBar;
  }

  //fill transformation
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

