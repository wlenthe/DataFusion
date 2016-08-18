/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                             *
 * Copyright (c) 2015 William Lenthe                                           *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.       *
 *                                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#include "MatchFeatureIdsGeometry.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"

#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

// Include the MOC generated file for this class
#include "moc_MatchFeatureIdsGeometry.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIdsGeometry::MatchFeatureIdsGeometry() :
  MatchFeatureIds(),
  m_ReferenceQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_MovingQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_ReferencePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_MovingPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_ReferenceCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MovingCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_OverlapArrayName(DataFusionConstants::SimilarityCoefficient),
  m_Metric(1),
  m_MetricThreshold(0.5),
  m_UseOrientations(false),
  m_OrientationTolerance(5.0),
  m_Overlap(NULL),
  m_ReferenceQuats(NULL),
  m_MovingQuats(NULL),
  m_ReferencePhases(NULL),
  m_MovingPhases(NULL),
  m_ReferenceCrystalStructures(NULL),
  m_MovingCrystalStructures(NULL)
{
  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIdsGeometry::~MatchFeatureIdsGeometry()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsGeometry::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  QVector<QString> choices;
  choices.push_back("Jaccard");
  choices.push_back("Sorensen-Dice");
  choices.push_back("Ochiai (cosine)");
  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
  parameter->setHumanLabel("Similairty Coefficient");
  parameter->setPropertyName("Metric");
  parameter->setChoices(choices);
  parameter->setCategory(FilterParameter::Parameter);
  parameters.push_back(parameter);
  parameters.push_back(DoubleFilterParameter::New("Minimum Metric Value", "MetricThreshold", getMetricThreshold(), FilterParameter::Parameter));

  parameters.push_back(StringFilterParameter::New("Matched Similarity Coefficient", "OverlapArrayName", getOverlapArrayName(), FilterParameter::CreatedArray));

  QStringList linkedProps;
  linkedProps<<"OrientationTolerance"<<"ReferenceQuatsArrayPath"<<"ReferencePhasesArrayPath"<<"ReferenceCrystalStructuresArrayPath"<<"MovingQuatsArrayPath"<<"MovingPhasesArrayPath"<<"MovingCrystalStructuresArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Require Orientation Match", "UseOrientations", getUseOrientations(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Orientation Tolerance Angle (degrees)", "OrientationTolerance", getOrientationTolerance(), FilterParameter::Parameter));

  DataArraySelectionFilterParameter::RequirementType req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Features Average Quats", "ReferenceQuatsArrayPath", getReferenceQuatsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Features Average Quats", "MovingQuatsArrayPath", getMovingQuatsArrayPath(), FilterParameter::RequiredArray, req));

  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Features Phases", "ReferencePhasesArrayPath", getReferencePhasesArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Features Phases", "MovingPhasesArrayPath", getMovingPhasesArrayPath(), FilterParameter::RequiredArray, req));

  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Features Crystal Structures", "ReferenceCrystalStructuresArrayPath", getReferenceCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Features Crystal Structures", "MovingCrystalStructuresArrayPath", getMovingCrystalStructuresArrayPath(), FilterParameter::RequiredArray, req));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsGeometry::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  MatchFeatureIds::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setMetric( reader->readValue("Metric", getMetric()) );
  setMetricThreshold( reader->readValue("MetricThreshold", getMetricThreshold() ) );
  setOverlapArrayName( reader->readString("OverlapArrayName", getOverlapArrayName() ) );

  setUseOrientations(reader->readValue("UseOrientations", getUseOrientations() ) );
  setOrientationTolerance(reader->readValue("OrientationTolerance", getOrientationTolerance() ) );
  setReferenceQuatsArrayPath( reader->readDataArrayPath( "ReferenceQuatsArrayPath", getReferenceQuatsArrayPath() ) );
  setReferencePhasesArrayPath( reader->readDataArrayPath( "ReferencePhasesArrayPath", getReferencePhasesArrayPath() ) );
  setReferenceCrystalStructuresArrayPath( reader->readDataArrayPath( "ReferenceCrystalStructuresArrayPath", getReferenceCrystalStructuresArrayPath() ) );
  setMovingQuatsArrayPath( reader->readDataArrayPath( "MovingQuatsArrayPath", getMovingQuatsArrayPath() ) );
  setMovingPhasesArrayPath( reader->readDataArrayPath( "MovingPhasesArrayPath", getMovingPhasesArrayPath() ) );
  setMovingCrystalStructuresArrayPath( reader->readDataArrayPath( "MovingCrystalStructuresArrayPath", getMovingCrystalStructuresArrayPath() ) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchFeatureIdsGeometry::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  MatchFeatureIds::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)

  SIMPL_FILTER_WRITE_PARAMETER(Metric)
  SIMPL_FILTER_WRITE_PARAMETER(MetricThreshold)
  SIMPL_FILTER_WRITE_PARAMETER(OverlapArrayName)

  SIMPL_FILTER_WRITE_PARAMETER(UseOrientations)
  SIMPL_FILTER_WRITE_PARAMETER(OrientationTolerance)
  SIMPL_FILTER_WRITE_PARAMETER(ReferenceQuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(ReferencePhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(ReferenceCrystalStructuresArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MovingQuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MovingPhasesArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MovingCrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsGeometry::dataCheck()
{
  setErrorCondition(0);
  MatchFeatureIds::dataCheck();

  DataArrayPath refIdspath = getReferenceFeatureIdsArrayPath();
  DataArrayPath movIdspath = getMovingFeatureIdsArrayPath();
  if(refIdspath.getDataContainerName() != movIdspath.getDataContainerName() || refIdspath.getAttributeMatrixName() != movIdspath.getAttributeMatrixName()) {
    notifyErrorMessage(getHumanLabel(), "Moving and Reference Feature Ids must belong to same Attribute Matrix", -1001);
  }

  if(getUseOrientations())
  {
    QVector<size_t> dims(1, 4);
    QVector<DataArrayPath> referenceDataArrayPaths, movingDataArrayPaths;
    m_ReferenceQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceQuatsArrayPath(), dims);
    if( NULL != m_ReferenceQuatsPtr.lock().get() )
    { m_ReferenceQuats = m_ReferenceQuatsPtr.lock()->getPointer(0); }
    if(getErrorCondition() >= 0) { referenceDataArrayPaths.push_back(getReferenceQuatsArrayPath()); }

    m_MovingQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingQuatsArrayPath(), dims);
    if( NULL != m_MovingQuatsPtr.lock().get() )
    { m_MovingQuats = m_MovingQuatsPtr.lock()->getPointer(0); }
    if(getErrorCondition() >= 0) { movingDataArrayPaths.push_back(getMovingQuatsArrayPath()); }

    dims[0]=1;
    m_ReferencePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferencePhasesArrayPath(), dims);
    if( NULL != m_ReferencePhasesPtr.lock().get() )
    { m_ReferencePhases = m_ReferencePhasesPtr.lock()->getPointer(0); }
    if(getErrorCondition() >= 0) { referenceDataArrayPaths.push_back(getReferencePhasesArrayPath()); }

    m_MovingPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingPhasesArrayPath(), dims);
    if( NULL != m_MovingPhasesPtr.lock().get() )
    { m_MovingPhases = m_MovingPhasesPtr.lock()->getPointer(0); }
    if(getErrorCondition() >= 0) { movingDataArrayPaths.push_back(getMovingPhasesArrayPath()); }

    m_ReferenceCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getReferenceCrystalStructuresArrayPath(), dims);
    if( NULL != m_ReferenceCrystalStructuresPtr.lock().get() )
    { m_ReferenceCrystalStructures = m_ReferenceCrystalStructuresPtr.lock()->getPointer(0); }

    m_MovingCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getMovingCrystalStructuresArrayPath(), dims);
    if( NULL != m_MovingCrystalStructuresPtr.lock().get() )
    { m_MovingCrystalStructures = m_MovingCrystalStructuresPtr.lock()->getPointer(0); }
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, referenceDataArrayPaths);
    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, movingDataArrayPaths);
  }
  if(getErrorCondition() < 0) return;

  //created arrays
  DataArrayPath tempPath;
  QVector<size_t> dims(1, 1);
  tempPath.update(getMovingCellFeatureAttributeMatrixPath().getDataContainerName(), getMovingCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getOverlapArrayName() );
  m_OverlapPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
  if( NULL != m_OverlapPtr.lock().get() )
  { m_Overlap = m_OverlapPtr.lock()->getPointer(0); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsGeometry::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

std::vector<size_t> MatchFeatureIdsGeometry::match() {
  //get sizes
  int64_t totalPoints = getReferenceFeatureIdsPtr().lock()->getNumberOfTuples();
  int maxReferenceId = getReferenceUniquePtr().lock()->getNumberOfTuples();
  int maxMovingId = getMovingUniquePtr().lock()->getNumberOfTuples();

  //reinterpret quats
  QuatF* reference_quats = reinterpret_cast<QuatF*>(m_ReferenceQuats);
  QuatF* moving_quats = reinterpret_cast<QuatF*>(m_MovingQuats);

  //initially everything but grain 0 is unique (no grains have been matched)
  std::fill(getReferenceUnique(), getReferenceUnique() + maxReferenceId, true);
  std::fill(getMovingUnique(), getMovingUnique() + maxMovingId, true);
  std::fill(m_Overlap, m_Overlap + maxMovingId, 0.0f);
  getReferenceUnique()[0] = false;
  getMovingUnique()[0] = false;

  //loop over volume finding intersections and volumes
  union PairIndex {
    int64_t index;
    int32_t ids[2];
    const bool operator< (const PairIndex &other) const {return index < other.index;}
  };
  std::map<PairIndex, size_t> intersections;
  std::vector<size_t> refVol(maxReferenceId + 1, 0);
  std::vector<size_t> movVol(maxMovingId + 1, 0);
  for(int64_t i = 0; i < totalPoints; i++) {
    int32_t refID = getReferenceFeatureIds()[i];
    int32_t movID = getMovingFeatureIds()[i];
    if(refID > 0 && movID > 0) {
      refVol[refID]++;
      movVol[movID]++;
      PairIndex p;
      p.ids[0] = refID;
      p.ids[1] = movID;
      intersections[p]++;
    }
  }

  //compute selected metric for each pair of overlapping grains
  class OverlapPair {
   public:
    int referenceId;
    int movingId;
    float index;
    const bool operator< (const OverlapPair &other) const {return index < other.index;}
  };
  std::vector<OverlapPair> featureOverlaps;
  for(std::map<PairIndex, size_t>::iterator iter = intersections.begin(); iter != intersections.end(); ++iter) {
    int32_t refID = iter->first.ids[0];
    int32_t movID = iter->first.ids[1];

    //if orientation match is required, only accumulate pairs that have matching crystal structures (phase is insufficient since they may have different cell ensemble matricies)
    if( !m_UseOrientations || (m_UseOrientations && m_ReferenceCrystalStructures[ m_ReferencePhases[refID] ] == m_MovingCrystalStructures[ m_MovingPhases[movID] ]) ) {
      OverlapPair overlapPair;
      overlapPair.referenceId = refID;
      overlapPair.movingId = movID;
      size_t overlap = iter->second;
      switch(m_Metric) {
        case 0: {//jaccard
          overlapPair.index = (float)overlap / (movVol[movID] + refVol[refID] - overlap);
        } break;

        case 1: {//dice
          overlapPair.index = (float)(2 * overlap) / (movVol[movID] + refVol[refID]);
        } break;

        case 2: {//cosine
          overlapPair.index = (float)overlap / sqrt(movVol[movID] * refVol[refID]);
        } break;
      }
      if(overlapPair.index >= m_MetricThreshold) featureOverlaps.push_back(overlapPair);
    }
  }
  std::sort(featureOverlaps.begin(), featureOverlaps.end());

  // create map of current to new moving grain ids
  std::vector<size_t> idMap(maxMovingId, -1);
  idMap[0] = 0;

  // begin assigning grains by overlap until (i) all moving grains have been assigned or (ii) overlapping pairs are exhausted
  float orientationTolerance = m_OrientationTolerance*SIMPLib::Constants::k_Pi/180.0f;
  int assignedGrains = 0;
  while(assignedGrains<maxMovingId && featureOverlaps.size()>0) {
    //get last pair (highest index / most overlap)
    int referenceId = featureOverlaps.back().referenceId;
    int movingId = featureOverlaps.back().movingId;
    bool assign = false;

    //only check for match if both grains are still available
    if(getReferenceUnique()[referenceId] && getMovingUnique()[movingId]) {
      if(m_UseOrientations) {
        //already checked for same structure during
        int referencePhase = m_ReferenceCrystalStructures[m_ReferencePhases[referenceId]];
        int movingPhase = m_MovingCrystalStructures[m_MovingPhases[movingId]];
        if(referencePhase==movingPhase && referencePhase!=Ebsd::CrystalStructure::UnknownCrystalStructure) {
          float w, n1, n2, n3;
          QuatF q1, q2;
          QuaternionMathF::Copy(reference_quats[referenceId], q1);
          QuaternionMathF::Copy(moving_quats[movingId], q2);
          w = m_OrientationOps[referencePhase]->getMisoQuat( q1, q2, n1, n2, n3);
          if(w<=orientationTolerance) {
            assign=true;
          }
        }
      } else {
        assign = true;
      }
    }

    if(assign) {
      idMap[movingId] = referenceId;
      getReferenceUnique()[referenceId] = false;
      getMovingUnique()[movingId] = false;
      m_Overlap[movingId] = featureOverlaps.back().index;
      assignedGrains++;
    }

    //remove from list
    featureOverlaps.pop_back();
  }
  return idMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsGeometry::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) {return;}
  if(getCancel() == true) {return;}
  MatchFeatureIds::execute();
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchFeatureIdsGeometry::newFilterInstance(bool copyFilterParameters)
{
  MatchFeatureIdsGeometry::Pointer filter = MatchFeatureIdsGeometry::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsGeometry::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsGeometry::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsGeometry::getHumanLabel()
{ return "Match Feature Ids (Geometry)"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsGeometry::getSubGroupName()
{ return "DataFusion"; }

