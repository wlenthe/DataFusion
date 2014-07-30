/*
 * Your License or Copyright Information can go here
 */

#include "MatchFeatureIds.h"

#include <QtCore/QString>

#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"

#include "DatasetMerging/DatasetMergingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIds::MatchFeatureIds() :
  AbstractFilter(),
  m_ReferenceFeatureIds(NULL),
  m_ReferenceQuats(NULL),
  m_ReferencePhases(NULL),
  m_ReferenceCrystalStructures(NULL),
  m_ReferenceFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_ReferenceQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_ReferencePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_ReferenceCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MovingFeatureIds(NULL),
  m_MovingQuats(NULL),
  m_MovingPhases(NULL),
  m_MovingCrystalStructures(NULL),
  m_MovingFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_MovingQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_MovingPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_MovingCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_UseOrientations(false),
  m_OrientationTolerance(5.0f),
  m_Metric(1),
  m_MetricThreshold(0.1f),
  m_CellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, "")
/* DO NOT FORGET TO INITIALIZE ALL YOUR DREAM3D Filter Parameters HERE */
{
  m_OrientationOps = OrientationOps::getOrientationOpsVector();
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
  parameters.push_back(FilterParameter::New("Reference Feature Ids", "ReferenceFeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceFeatureIdsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Feature Ids", "MovingFeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingFeatureIdsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Cell Attribute Matrix", "CellFeatureAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getCellFeatureAttributeMatrixPath(), false));
  
  QVector<QString> choices;
    choices.push_back("Jaccard");
    choices.push_back("Sorensen-Dice");
    choices.push_back("Ochiai (cosine)");  
  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Similairty Coefficient");
    parameter->setPropertyName("Metric");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setChoices(choices);
  parameters.push_back(parameter);
  parameters.push_back(FilterParameter::New("Minimum Metric Value", "MetricThreshold", FilterParameterWidgetType::DoubleWidget, getMetricThreshold(), false, ""));

  QStringList linkedProps;
  linkedProps<<"OrientationTolerance"<<"ReferenceQuatsArrayPath"<<"ReferencePhasesArrayPath"<<"ReferenceCrystalStructuresArrayPath"<<"MovingQuatsArrayPath"<<"MovingPhasesArrayPath"<<"MovingCrystalStructuresArrayPath";
  parameters.push_back(FilterParameter::NewConditional("Require Orientation Match", "UseOrientations", FilterParameterWidgetType::LinkedBooleanWidget, getUseOrientations(), false, linkedProps));
  parameters.push_back(FilterParameter::New("Orientation Tolerance Angle (degrees)", "OrientationTolerance", FilterParameterWidgetType::DoubleWidget, getOrientationTolerance(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Features Average Quats", "ReferenceQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceQuatsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Features Phases", "ReferencePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferencePhasesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Features Crystal Structures", "ReferenceCrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceCrystalStructuresArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Features Average Quats", "MovingQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingQuatsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Features Phases", "MovingPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingPhasesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Features Crystal Structures", "MovingCrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingCrystalStructuresArrayPath(), false, ""));
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
  setCellFeatureAttributeMatrixPath( reader->readDataArrayPath("CellFeatureAttributeMatrixPath", getCellFeatureAttributeMatrixPath() ) );
  
  setMetric( reader->readValue("Metric", getMetric()) );
  setMetricThreshold( reader->readValue("MetricThreshold", getMetricThreshold() ) );

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
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceFeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingFeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CellFeatureAttributeMatrixPath)

  DREAM3D_FILTER_WRITE_PARAMETER(Metric)
  DREAM3D_FILTER_WRITE_PARAMETER(MetricThreshold)
  
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
  QVector<size_t> dims(1, 1);
  m_ReferenceFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferenceFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ReferenceFeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ReferenceFeatureIds = m_ReferenceFeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_MovingFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MovingFeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MovingFeatureIds = m_MovingFeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getUseOrientations())
  {
    dims[0]=4;
    m_ReferenceQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ReferenceQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_ReferenceQuats = m_ReferenceQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_MovingQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingQuatsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_MovingQuatsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_MovingQuats = m_MovingQuatsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    dims[0]=1;
    m_ReferencePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferencePhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ReferencePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_ReferencePhases = m_ReferencePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_MovingPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingPhasesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_MovingPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_MovingPhases = m_MovingPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    m_ReferenceCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getReferenceCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ReferenceCrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_ReferenceCrystalStructures = m_ReferenceCrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    m_MovingCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getMovingCrystalStructuresArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_MovingCrystalStructuresPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_MovingCrystalStructures = m_MovingCrystalStructuresPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
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
const QString MatchFeatureIds::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getGroupName()
{
  return "DatasetMerging";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getHumanLabel()
{
  return "Match Feature Ids";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIds::getSubGroupName()
{
  return "Misc";
}

// custom class to handle holding of pair data
class OverlapPair {
 public:
  int referenceId;
  int movingId;
  float index;
  
    // I'm defining the < operator 'backwards' ---> std::sort from lowest to highest cost (so I can pop_back)
    const bool operator< (const OverlapPair &other) const
    {
      return index < other.index;
    } 
};
#include <windows.h>
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIds::execute()
{
  int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  int64_t totalPoints = m_ReferenceFeatureIdsPtr.lock()->getNumberOfTuples();
  QuatF* reference_quats = reinterpret_cast<QuatF*>(m_ReferenceQuats);
  QuatF* moving_quats = reinterpret_cast<QuatF*>(m_MovingQuats);

  // find highest grain id in both sets set
  int maxReferenceId = 0;
  int maxMovingId = 0;
  for(int i=0; i<totalPoints; i++)
  {
    if(m_ReferenceFeatureIds[i]>maxReferenceId)
    {
      maxReferenceId=m_ReferenceFeatureIds[i];
    }
    if(m_MovingFeatureIds[i]>maxMovingId)
    {
      maxMovingId=m_MovingFeatureIds[i];
    }
  }

  // create array to hold grain overlaps (ignoring grain 0)
  std::vector<int> intersections(maxReferenceId*maxMovingId, 0);

  // create arrays to hold grain volumes
  std::vector<int> referenceVolumes(maxReferenceId, 0);
  std::vector<int> movingVolumes(maxMovingId, 0);

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
  std::vector<OverlapPair> featureOverlaps;
  for(int i=0; i<maxMovingId; i++)
  {
    int index = i*maxReferenceId;
    for(int j=0; j<maxReferenceId; j++)
    {
      int overlap = intersections[index+j];
      if(overlap>0)
      {
        OverlapPair overlapPair;
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
            overlapPair.index = (float)overlap/sqrt(movingVolumes[i]*movingVolumes[i]+referenceVolumes[j]*referenceVolumes[j]);
          } break;
        }
        if(overlapPair.index>=m_MetricThreshold) featureOverlaps.push_back(overlapPair);
      }
    }
  }
  std::sort(featureOverlaps.begin(), featureOverlaps.end());

  // create map of moving to reference grains
  std::vector<size_t> idMap(maxMovingId+1, 0);

  // begin assigning grains by overlap until (i) all moving grains have been assigned or (ii) overlapping pairs are exhausted
  std::vector<bool> referenceAssigned(maxReferenceId+1, false);
  std::vector<bool> movingAssigned(maxMovingId+1, false);
  float orientationTolerance = m_OrientationTolerance*DREAM3D::Constants::k_Pi/180.0f;
  int assignedGrains = 0;
  while(assignedGrains<maxMovingId && featureOverlaps.size()>0)
  {
    //get last pair (highest index / most overlap)
    int referenceId = featureOverlaps.back().referenceId;
    int movingId = featureOverlaps.back().movingId;
    bool assign = false;
    if(!referenceAssigned[referenceId] && !movingAssigned[movingId])
    {
      if(m_UseOrientations)
      {
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
      referenceAssigned[referenceId]=true;
      movingAssigned[movingId]=true;
      assignedGrains++;
    }

    //remove from list
    featureOverlaps.pop_back();
  }

  // if all the grains haven't been assigned, there are some with no overlap (append)
  int index = maxReferenceId;
  if(assignedGrains!=maxMovingId)
  {
    for(int i=0; i<maxMovingId; i++)
    {
      if(!movingAssigned[i+1])
      {
        index++;
        idMap[i+1] = index;
      }
    }
  }

  // modify moving feature ids
  for(int i=0; i<totalPoints; i++)
  {
    m_MovingFeatureIds[i]=idMap[m_MovingFeatureIds[i]];
  }

  // resize and rearrange moving feature attribute arrays
  QVector<size_t> tDims(1,index+1);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getCellFeatureAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getCellFeatureAttributeMatrixPath().getAttributeMatrixName());
  cellFeatureAttrMat->setTupleDimensions(tDims);

  QList<QString> featureArrayNames = cellFeatureAttrMat->getAttributeArrayNameList();
  idMap.resize(tDims[0],0);
  QVector<size_t> newOrder = QVector<size_t>::fromStdVector(idMap);
  for(QList<QString>::iterator iter = featureArrayNames.begin(); iter != featureArrayNames.end(); ++iter)
  {
    IDataArray::Pointer pOld = cellFeatureAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer pNew = pOld->reorderCopy(newOrder);
    cellFeatureAttrMat->removeAttributeArray(*iter);
    cellFeatureAttrMat->addAttributeArray(pNew->getName(), pNew);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchFeatureIds::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  MatchFeatureIds::Pointer filter = MatchFeatureIds::New();
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

