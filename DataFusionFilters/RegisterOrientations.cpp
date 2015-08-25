/*
 * Your License or Copyright can go here
 */

#include "RegisterOrientations.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegisterOrientations::RegisterOrientations() :
  AbstractFilter(),
  m_ReferenceAvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_MovingAvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_ReferencePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_MovingPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_ReferenceCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MovingCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_ReferenceGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_MovingGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_ReferenceAvgQuats(NULL),
  m_MovingAvgQuats(NULL),
  m_ReferencePhases(NULL),
  m_MovingPhases(NULL),
  m_ReferenceCrystalStructures(NULL),
  m_MovingCrystalStructures(NULL),
  m_ReferenceGoodFeatures(NULL),
  m_MovingGoodFeatures(NULL),
  m_UseGoodFeatures(true),
  m_MinMiso(1.0)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegisterOrientations::~RegisterOrientations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::DataStructureRequirements req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Average Quats", "ReferenceAvgQuatsArrayPath", getReferenceAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Average Quats", "MovingAvgQuatsArrayPath", getMovingAvgQuatsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Feature Phases", "ReferencePhasesArrayPath", getReferencePhasesArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Feature Phases", "MovingPhasesArrayPath", getMovingPhasesArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Crystal Structures", "ReferenceCrystalStructuresArrayPath", getReferenceCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Crystal Structures", "MovingCrystalStructuresArrayPath", getMovingCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  {
    QStringList linkedProps;
    linkedProps << "ReferenceGoodFeaturesArrayPath" << "MovingGoodFeaturesArrayPath";
    parameters.push_back(LinkedBooleanFilterParameter::New("Use Good Features", "UseGoodFeatures", getUseGoodFeatures(), linkedProps, FilterParameter::Parameter));
  }
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Good Features", "ReferenceGoodFeaturesArrayPath", getReferenceGoodFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Good Features", "MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DoubleFilterParameter::New("Minimum Average Rotation Angle", "MinMiso", getMinMiso(), FilterParameter::Parameter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceAvgQuatsArrayPath( reader->readDataArrayPath("ReferenceAvgQuatsArrayPath", getReferenceAvgQuatsArrayPath() ) );
  setMovingAvgQuatsArrayPath( reader->readDataArrayPath("MovingAvgQuatsArrayPath", getMovingAvgQuatsArrayPath() ) );
  setReferenceGoodFeaturesArrayPath( reader->readDataArrayPath("ReferenceGoodFeaturesArrayPath", getReferenceGoodFeaturesArrayPath() ) );
  setUseGoodFeatures( reader->readValue("UseGoodFeatures", getUseGoodFeatures() ) );
  setMovingGoodFeaturesArrayPath( reader->readDataArrayPath("MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath() ) );
  setReferencePhasesArrayPath( reader->readDataArrayPath("ReferencePhasesArrayPath", getReferencePhasesArrayPath() ) );
  setMovingPhasesArrayPath( reader->readDataArrayPath("MovingPhasesArrayPath", getMovingPhasesArrayPath() ) );
  setReferenceCrystalStructuresArrayPath( reader->readDataArrayPath("ReferenceCrystalStructuresArrayPath", getReferenceCrystalStructuresArrayPath() ) );
  setMovingCrystalStructuresArrayPath( reader->readDataArrayPath("MovingCrystalStructuresArrayPath", getMovingCrystalStructuresArrayPath() ) );
  setMinMiso( reader->readValue("MinMiso", getMinMiso() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegisterOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceAvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingAvgQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodFeatures)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferencePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MinMiso)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterOrientations::dataCheck()
{
  setErrorCondition(0);

  //prereq arrays
  QVector<size_t> dims(1, 4);
  QVector<DataArrayPath> referenceDataArrayPaths, movingDataArrayPaths;
  m_ReferenceAvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceAvgQuatsArrayPath(), dims);
  if( NULL != m_ReferenceAvgQuatsPtr.lock().get() ) m_ReferenceAvgQuats = m_ReferenceAvgQuatsPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getReferenceAvgQuatsArrayPath());
  m_MovingAvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingAvgQuatsArrayPath(), dims);
  if( NULL != m_MovingAvgQuatsPtr.lock().get() ) m_MovingAvgQuats = m_MovingAvgQuatsPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) movingDataArrayPaths.push_back(getMovingAvgQuatsArrayPath());

  dims[0] = 1;
  m_ReferencePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferencePhasesArrayPath(), dims);
  if( NULL != m_ReferencePhasesPtr.lock().get() ) m_ReferencePhases = m_ReferencePhasesPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getReferencePhasesArrayPath());
  m_MovingPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingPhasesArrayPath(), dims);
  if( NULL != m_MovingPhasesPtr.lock().get() ) m_MovingPhases = m_MovingPhasesPtr.lock()->getPointer(0);
  if(getErrorCondition() >= 0) movingDataArrayPaths.push_back(getMovingPhasesArrayPath());

  m_ReferenceCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getReferenceCrystalStructuresArrayPath(), dims);
  if( NULL != m_ReferenceCrystalStructuresPtr.lock().get() ) m_ReferenceCrystalStructures = m_ReferenceCrystalStructuresPtr.lock()->getPointer(0);
  m_MovingCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getMovingCrystalStructuresArrayPath(), dims);
  if( NULL != m_MovingCrystalStructuresPtr.lock().get() ) m_MovingCrystalStructures = m_MovingCrystalStructuresPtr.lock()->getPointer(0);

  if(getUseGoodFeatures())
  {
    m_ReferenceGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getReferenceGoodFeaturesArrayPath(), dims);
    if( NULL != m_ReferenceGoodFeaturesPtr.lock().get() ) m_ReferenceGoodFeatures = m_ReferenceGoodFeaturesPtr.lock()->getPointer(0);
    if(getErrorCondition() >= 0) referenceDataArrayPaths.push_back(getReferenceGoodFeaturesArrayPath());
    m_MovingGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMovingGoodFeaturesArrayPath(), dims);
    if( NULL != m_MovingGoodFeaturesPtr.lock().get() ) m_MovingGoodFeatures = m_MovingGoodFeaturesPtr.lock()->getPointer(0);
    if(getErrorCondition() >= 0) movingDataArrayPaths.push_back(getMovingGoodFeaturesArrayPath());
  }
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, referenceDataArrayPaths);
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, movingDataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterOrientations::preflight()
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
void RegisterOrientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  //convert from float* to quat*
  QuatF* movingQuats = reinterpret_cast<QuatF*>(m_MovingAvgQuats);
  QuatF* referenceQuats = reinterpret_cast<QuatF*>(m_ReferenceAvgQuats);

  //create list of shared grains
  size_t referenceNumFeatures = m_ReferencePhasesPtr.lock()->getNumberOfTuples();
  size_t movingNumFeatures = m_MovingPhasesPtr.lock()->getNumberOfTuples();
  size_t numFeatures = std::min(referenceNumFeatures, movingNumFeatures);
  std::vector<size_t> sharedFeatures;
  sharedFeatures.reserve(numFeatures);

  for(size_t i = 0; i<numFeatures; i++)
  {
    if(getUseGoodFeatures())
    {
      if(m_ReferenceGoodFeatures[i] && m_MovingGoodFeatures[i])
        continue;
    }
    if(m_ReferenceCrystalStructures[m_ReferencePhases[i]] == m_MovingCrystalStructures[m_MovingPhases[i]])
      sharedFeatures.push_back(i);
  }

  //build symmetry quat list for all structures
  std::vector< std::vector<QuatF> > symOpsList(m_OrientationOps.size());
  for(int i = 0; i < m_OrientationOps.size(); i++)
  {
    for(int j = 0; j < m_OrientationOps[i]->getNumSymOps(); j++)
    {
      QuatF symQuat;
      m_OrientationOps[i]->getQuatSymOp(j, symQuat);
      symOpsList[i].push_back(symQuat);
    }
  }

  //loop over matched features determining best rotation from each pair (testing against all other pairs)
  std::vector<QuatF> bestQuats(sharedFeatures.size());
  std::vector<float> avgMisorientations(sharedFeatures.size());
  for(size_t i = 0; i < sharedFeatures.size(); i++)
  {
    //compute difference in orientation
    float minAvgMiso = 999.9f;
    QuatF gMovInv, candidateQuat, bestQuat;
    QuaternionMathF::Copy(movingQuats[sharedFeatures[i]], gMovInv);
    QuaternionMathF::Conjugate(gMovInv);
    uint32_t xtal = m_ReferenceCrystalStructures[m_ReferencePhases[sharedFeatures[i]]];//already checked that phases match

    //loop over symmetry operators for structure computing all possible equivilent ways of expressing rotation
    for(int j = 0; j < symOpsList[xtal].size(); j++)
    {
      QuatF tempQuat;
      float avgMiso = 0.0f;
      QuaternionMathF::Multiply(gMovInv, symOpsList[xtal][j], tempQuat);
      QuaternionMathF::Multiply(tempQuat, referenceQuats[sharedFeatures[i]], candidateQuat);

      //loop over all other pairs finding the resuling misorientation if the candidate rotation is applied to each
      for(size_t k = 0; k < sharedFeatures.size(); k++)
      {
        if(k != i)
        {
          QuatF otherGMov, otherGRef, otherGMovRotInv;
          QuaternionMathF::Multiply(movingQuats[sharedFeatures[k]], candidateQuat, otherGMovRotInv);
          QuaternionMathF::Conjugate(otherGMovRotInv);

          float cos_min_angle = 0.0f;
          for(int l = 0; l < symOpsList[xtal].size(); l++)
          {
            QuatF misoQuat;
            QuaternionMathF::Multiply(otherGMovRotInv, symOpsList[xtal][l], tempQuat);
            QuaternionMathF::Multiply(tempQuat, referenceQuats[sharedFeatures[k]], misoQuat);
            QuaternionMathF::ElementWiseAbs(misoQuat);

            if(misoQuat.w > cos_min_angle)
              cos_min_angle = misoQuat.w;
          }
          avgMiso += acos(std::min(cos_min_angle, 1.0f));
        }
      }
      avgMiso /= static_cast<float>(sharedFeatures.size() - 1);

      if(avgMiso < minAvgMiso)
      {
        minAvgMiso = avgMiso;
        QuaternionMathF::Copy(candidateQuat, bestQuat);
      }
    }

    //convert the average misorientation of the best match to degrees and add best match to list of matches
    if(bestQuat.w < 0) QuaternionMathF::Negate(bestQuat);//only consider rotations 0-180
    QuaternionMathF::Copy(bestQuat, bestQuats[i]);
    avgMisorientations[i] = 2.0f * minAvgMiso;
  }

  //find average rotation
  /***************************************************************************************************************************************
   *
   * This should be done by converting to cubochoric, averaging, and converting back to a quat (once the orientation library supports it)
   *
   ***************************************************************************************************************************************/
  double minMisoAngle = getMinMiso() * M_PI / 180.0f;
  QuatF rotation = QuaternionMathF::New(0, 0, 0, 0);
  for(int i = 0; i<bestQuats.size(); i++)
  {
    if(avgMisorientations[i] < minMisoAngle)
      QuaternionMathF::Add(rotation, bestQuats[i], rotation);
  }

  if(0.0f == rotation.x && 0.0f == rotation.y && 0.0f == rotation.z && 0.0f == rotation.w)
  {
    notifyErrorMessage(getHumanLabel(), "failed to compute rotation (no rotations within tolerance)", -1);
    return;
  }

  //apply rotation to align quats 'moving' quats
  QuaternionMathF::UnitQuaternion(rotation);
  for(int i = 0; i < movingNumFeatures; i++)
  {
    QuatF correctedQuat;
    QuaternionMathF::Multiply(movingQuats[i], rotation, correctedQuat);
    QuaternionMathF::Copy(correctedQuat, movingQuats[i]);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegisterOrientations::newFilterInstance(bool copyFilterParameters)
{
  RegisterOrientations::Pointer filter = RegisterOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getHumanLabel()
{ return "Register Orientations"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getSubGroupName()
{ return "DataFusion"; }

