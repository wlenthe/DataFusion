/*
 *
 * Copyright (c) 2015 William Lenthe
 *
 * This file is part of This program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "MatchFeatureIds.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"
#include "DREAM3DLib/FilterParameters/LinkedBooleanFilterParameter.h"

#include "DREAM3DLib/FilterParameters/DataArrayCreationFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"
namespace Detail
{
// custom class to holding pair data
class OverlapPair {
 public:
  int referenceId;
  int movingId;
  float index;
  const bool operator< (const OverlapPair &other) const {return index < other.index;}
};
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIds::MatchFeatureIds() :
  AbstractFilter(),
  m_ReferenceCellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_MovingCellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_ReferenceFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_MovingFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_ReferenceQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_MovingQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_ReferencePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_MovingPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_ReferenceCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MovingCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_OverlapArrayName(DataFusionConstants::SimilarityCoefficient),
  m_ReferenceUniqueArrayName(DataFusionConstants::UniqueFeatures),
  m_MovingUniqueArrayName(DataFusionConstants::UniqueFeatures),
  m_Metric(1),
  m_MetricThreshold(0.5),
  m_UseOrientations(false),
  m_OrientationTolerance(5.0),
  m_Overlap(NULL),
  m_ReferenceUnique(NULL),
  m_MovingUnique(NULL),
  m_ReferenceFeatureIds(NULL),
  m_MovingFeatureIds(NULL),
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
MatchFeatureIds::~MatchFeatureIds()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIds::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::RequirementType req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Feature Ids", "ReferenceFeatureIdsArrayPath", getReferenceFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Feature Ids", "MovingFeatureIdsArrayPath", getMovingFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));

  AttributeMatrixSelectionFilterParameter::RequirementType amReq;
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Reference Cell Feature Attribute Matrix", "ReferenceCellFeatureAttributeMatrixPath", getReferenceCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, amReq));
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Moving Cell Feature Attribute Matrix", "MovingCellFeatureAttributeMatrixPath", getMovingCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, amReq));

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
  parameters.push_back(StringFilterParameter::New("Reference Unique Grains", "ReferenceUniqueArrayName", getReferenceUniqueArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Moving Unique Grains", "MovingUniqueArrayName", getMovingUniqueArrayName(), FilterParameter::CreatedArray));

  QStringList linkedProps;
  linkedProps<<"OrientationTolerance"<<"ReferenceQuatsArrayPath"<<"ReferencePhasesArrayPath"<<"ReferenceCrystalStructuresArrayPath"<<"MovingQuatsArrayPath"<<"MovingPhasesArrayPath"<<"MovingCrystalStructuresArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Require Orientation Match", "UseOrientations", getUseOrientations(), linkedProps, FilterParameter::Parameter));
  parameters.push_back(DoubleFilterParameter::New("Orientation Tolerance Angle (degrees)", "OrientationTolerance", getOrientationTolerance(), FilterParameter::Parameter));

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
void MatchFeatureIds::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setReferenceFeatureIdsArrayPath( reader->readDataArrayPath( "ReferenceFeatureIdsArrayPath", getReferenceFeatureIdsArrayPath() ) );
  setMovingFeatureIdsArrayPath( reader->readDataArrayPath( "MovingFeatureIdsArrayPath", getMovingFeatureIdsArrayPath() ) );
  setReferenceCellFeatureAttributeMatrixPath( reader->readDataArrayPath("ReferenceCellFeatureAttributeMatrixPath", getReferenceCellFeatureAttributeMatrixPath() ) );
  setMovingCellFeatureAttributeMatrixPath( reader->readDataArrayPath("MovingCellFeatureAttributeMatrixPath", getMovingCellFeatureAttributeMatrixPath() ) );

  setMetric( reader->readValue("Metric", getMetric()) );
  setMetricThreshold( reader->readValue("MetricThreshold", getMetricThreshold() ) );
  setReferenceUniqueArrayName( reader->readString("ReferenceUniqueArrayName", getReferenceUniqueArrayName() ) );
  setMovingUniqueArrayName( reader->readString("MovingUniqueArrayName", getMovingUniqueArrayName() ) );

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
int MatchFeatureIds::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceFeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingFeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCellFeatureAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCellFeatureAttributeMatrixPath)

  DREAM3D_FILTER_WRITE_PARAMETER(Metric)
  DREAM3D_FILTER_WRITE_PARAMETER(MetricThreshold)
  DREAM3D_FILTER_WRITE_PARAMETER(OverlapArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceUniqueArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingUniqueArrayName)

  DREAM3D_FILTER_WRITE_PARAMETER(UseOrientations)
  DREAM3D_FILTER_WRITE_PARAMETER(OrientationTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferencePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingQuatsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCrystalStructuresArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIds::dataCheck()
{
  setErrorCondition(0);

  //required attribute matricies
  AttributeMatrix::Pointer refCellFeatAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getReferenceCellFeatureAttributeMatrixPath(), -303);
  AttributeMatrix::Pointer moveCellFeatAttrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getMovingCellFeatureAttributeMatrixPath(), -303);
  if(getErrorCondition() < 0) return;

  if(DREAM3D::AttributeMatrixType::CellFeature !=refCellFeatAttrMat->getType() )
    notifyErrorMessage(getHumanLabel(), "'Reference Cell Feature Attribute Matrix' must be of type CellFeature", -1000);
  if(DREAM3D::AttributeMatrixType::CellFeature !=moveCellFeatAttrMat->getType() )
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Feature Attribute Matrix' must be of type CellFeature", -1000);
  if(getReferenceCellFeatureAttributeMatrixPath() == getMovingCellFeatureAttributeMatrixPath())
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Feature Attribute Matrix' and 'Reference Cell Feature Attribute Matrix' must be different", -1001);
  if(getErrorCondition() < 0) return;

  //required arrays
  QVector<size_t> dims(1, 1);
  QVector<DataArrayPath> referenceDataArrayPaths, movingDataArrayPaths;
  m_ReferenceFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferenceFeatureIdsArrayPath(), dims);
  if( NULL != m_ReferenceFeatureIdsPtr.lock().get() )
  { m_ReferenceFeatureIds = m_ReferenceFeatureIdsPtr.lock()->getPointer(0); }
  if(getErrorCondition() >= 0) { referenceDataArrayPaths.push_back(getReferenceFeatureIdsArrayPath()); }

  m_MovingFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingFeatureIdsArrayPath(), dims);
  if( NULL != m_MovingFeatureIdsPtr.lock().get() )
  { m_MovingFeatureIds = m_MovingFeatureIdsPtr.lock()->getPointer(0); }
  if(getErrorCondition() >= 0) { movingDataArrayPaths.push_back(getMovingFeatureIdsArrayPath()); }

  if(getReferenceFeatureIdsArrayPath() == getMovingCellFeatureAttributeMatrixPath())
    notifyErrorMessage(getHumanLabel(), "'Moving Feature Ids' and 'Reference Feature Ids' must be different", -1001);

  if(getUseOrientations())
  {
    dims[0]=4;
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
  }
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, referenceDataArrayPaths);
  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, movingDataArrayPaths);
  if(getErrorCondition() < 0) return;

  //created arrays
  DataArrayPath tempPath;
  tempPath.update(getMovingCellFeatureAttributeMatrixPath().getDataContainerName(), getMovingCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getOverlapArrayName() );
  m_OverlapPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, dims);
  if( NULL != m_OverlapPtr.lock().get() )
  { m_Overlap = m_OverlapPtr.lock()->getPointer(0); }

  tempPath.update(getReferenceCellFeatureAttributeMatrixPath().getDataContainerName(), getReferenceCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getReferenceUniqueArrayName() );
  m_ReferenceUniquePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, 0, dims);
  if( NULL != m_ReferenceUniquePtr.lock().get() )
  { m_ReferenceUnique = m_ReferenceUniquePtr.lock()->getPointer(0); }

  tempPath.update(getMovingCellFeatureAttributeMatrixPath().getDataContainerName(), getMovingCellFeatureAttributeMatrixPath().getAttributeMatrixName(), getMovingUniqueArrayName() );
  m_MovingUniquePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, 0, dims);
  if( NULL != m_MovingUniquePtr.lock().get() )
  { m_MovingUnique = m_MovingUniquePtr.lock()->getPointer(0); }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIds::preflight()
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
void MatchFeatureIds::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  int64_t totalPoints = m_ReferenceFeatureIdsPtr.lock()->getNumberOfTuples();
  QuatF* reference_quats = reinterpret_cast<QuatF*>(m_ReferenceQuats);
  QuatF* moving_quats = reinterpret_cast<QuatF*>(m_MovingQuats);

  int maxReferenceId = m_ReferenceUniquePtr.lock()->getNumberOfTuples();
  int maxMovingId = m_MovingUniquePtr.lock()->getNumberOfTuples();

  // create array to hold grain overlaps (ignoring grain 0)
  std::vector<int> intersections(maxReferenceId * maxMovingId, 0);

  // create arrays to hold grain volumes
  std::vector<int> referenceVolumes(maxReferenceId, 0);
  std::vector<int> movingVolumes(maxMovingId, 0);//initially everything but grain 0 unique (no grains have been matched)
  m_ReferenceUnique[0] = false;
  m_MovingUnique[0] = false;
  m_Overlap[0] = false;
  for(int i = 1; i < maxReferenceId; i++)
  {
    m_ReferenceUnique[i] = true;
  }
  for(int i = 1; i < maxMovingId; i++)
  {
    m_MovingUnique[i] = true;
    m_Overlap[i] = 0;
  }

  // loop over volume finding intersections and volumes
  for(int i=0; i<totalPoints; i++)
  {
    int referenceId = m_ReferenceFeatureIds[i];
    int movingId = m_MovingFeatureIds[i];
    if(referenceId>0 && movingId>0)
    {
      referenceId--;
      movingId--;
      referenceVolumes[referenceId]++;
      movingVolumes[movingId]++;
      intersections[movingId*maxReferenceId+referenceId]++;
    }
  }

  // compute selected metric for each pair of overlapping grains
  std::vector<Detail::OverlapPair> featureOverlaps;
  for(int i=0; i<maxMovingId; i++)
  {
    int index = i*maxReferenceId;
    for(int j=0; j<maxReferenceId; j++)
    {
      int overlap = intersections[index+j];
      //only consider pairs with finite overlap
      if(overlap>0)
      {
        //if orientation match is required, only accumulate pairs that have matching crystal structures (phase is insufficient since they may have different cell ensemble matricies)
        if( !m_UseOrientations || (m_UseOrientations && m_ReferenceCrystalStructures[ m_ReferencePhases[j+1] ] == m_MovingCrystalStructures[ m_MovingPhases[i+1] ]) )
        {
          Detail::OverlapPair overlapPair;
          overlapPair.referenceId = j+1;
          overlapPair.movingId = i+1;

          switch(m_Metric)
          {
            case 0://jaccard
            {
              overlapPair.index = (float)overlap/(movingVolumes[i]+referenceVolumes[j]-overlap);
            } break;

            case 1://dice
            {
              overlapPair.index = (float)(2*overlap)/(movingVolumes[i]+referenceVolumes[j]);
            } break;

            case 2://cosine
            {
              overlapPair.index = (float)overlap/sqrt(movingVolumes[i]*referenceVolumes[j]);
            } break;
          }
          if(overlapPair.index>=m_MetricThreshold) featureOverlaps.push_back(overlapPair);
        }
      }
    }
  }
  std::sort(featureOverlaps.begin(), featureOverlaps.end());

  // create map of current to new moving grain ids
  std::vector<size_t> idMap(maxMovingId, -1);
  idMap[0] = 0;

  // begin assigning grains by overlap until (i) all moving grains have been assigned or (ii) overlapping pairs are exhausted
  float orientationTolerance = m_OrientationTolerance*DREAM3D::Constants::k_Pi/180.0f;
  int assignedGrains = 0;
  while(assignedGrains<maxMovingId && featureOverlaps.size()>0)
  {
    //get last pair (highest index / most overlap)
    int referenceId = featureOverlaps.back().referenceId;
    int movingId = featureOverlaps.back().movingId;
    bool assign = false;

    //only check for match if both grains are still available
    if(m_ReferenceUnique[referenceId] && m_MovingUnique[movingId])
    {
      if(m_UseOrientations)
      {
        //already checked for same structure during
        int referencePhase = m_ReferenceCrystalStructures[m_ReferencePhases[referenceId]];
        int movingPhase = m_MovingCrystalStructures[m_MovingPhases[movingId]];
        if(referencePhase==movingPhase && referencePhase!=Ebsd::CrystalStructure::UnknownCrystalStructure)
        {
          float w, n1, n2, n3;
          QuatF q1, q2;
          QuaternionMathF::Copy(reference_quats[referenceId], q1);
          QuaternionMathF::Copy(moving_quats[movingId], q2);
          w = m_OrientationOps[referencePhase]->getMisoQuat( q1, q2, n1, n2, n3);
          if(w<=orientationTolerance)
          {
            assign=true;
          }
        }
      }
      else
      {
        assign = true;
      }
    }

    if(assign)
    {
      idMap[movingId] = referenceId;
      m_ReferenceUnique[referenceId] = false;
      m_MovingUnique[movingId] = false;
      m_Overlap[movingId] = featureOverlaps.back().index;
      assignedGrains++;
    }

    //remove from list
    featureOverlaps.pop_back();
  }


  //if all the moving grains haven't been assigned append to higher grain ids
  int index = maxReferenceId;
  for(size_t i = 1; i < maxMovingId; i++)
  {
    if(-1 == idMap[i])
    {
      idMap[i] = index;
      index++;
    }
  }

  // modify moving feature ids
  for(int i=0; i<totalPoints; i++)
  {
    m_MovingFeatureIds[i]=idMap[m_MovingFeatureIds[i]];
  }

  // resize and rearrange moving feature attribute arrays
  QVector<size_t> tDims(1,index);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getMovingCellFeatureAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getMovingCellFeatureAttributeMatrixPath().getAttributeMatrixName());
  cellFeatureAttrMat->setTupleDimensions(tDims);

  QList<QString> featureArrayNames = cellFeatureAttrMat->getAttributeArrayNames();
  idMap.resize(tDims[0],0);
  QVector<size_t> newOrder = QVector<size_t>::fromStdVector(idMap);
  for(QList<QString>::iterator iter = featureArrayNames.begin(); iter != featureArrayNames.end(); ++iter)
  {
    IDataArray::Pointer pOld = cellFeatureAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer pNew = pOld->reorderCopy(newOrder);
    cellFeatureAttrMat->removeAttributeArray(*iter);
    cellFeatureAttrMat->addAttributeArray(pNew->getName(), pNew);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchFeatureIds::newFilterInstance(bool copyFilterParameters)
{
  MatchFeatureIds::Pointer filter = MatchFeatureIds::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getHumanLabel()
{ return "Match Feature Ids"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getSubGroupName()
{ return "DataFusion"; }

