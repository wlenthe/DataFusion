/*
 * Your License or Copyright Information can go here
 */

#include "RegisterOrientations.h"

#include <QtCore/QString>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DatasetMerging/DatasetMergingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RegisterOrientations::RegisterOrientations() :
  AbstractFilter(),
  m_ReferenceAvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_MovingAvgQuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_ReferenceAvgQuats(NULL),
  m_MovingAvgQuats(NULL),
  m_ReferencePhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_MovingPhasesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::Phases),
  m_ReferencePhases(NULL),
  m_MovingPhases(NULL),
  m_ReferenceCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_MovingCrystalStructuresArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_ReferenceCrystalStructures(NULL),
  m_MovingCrystalStructures(NULL),
  m_ReferenceGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_MovingGoodFeaturesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::GoodFeatures),
  m_ReferenceGoodFeatures(NULL),
  m_MovingGoodFeatures(NULL),
  m_WriteOutputs(false),
  m_OutputFile(""),
  m_MinMiso(1.0)
{
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
  parameters.push_back(FilterParameter::New("Reference Quats", "ReferenceAvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceAvgQuatsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Quats", "MovingAvgQuatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingAvgQuatsArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Phases", "ReferencePhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferencePhasesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Phases", "MovingPhasesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingPhasesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Crystal Structures", "ReferenceCrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceCrystalStructuresArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Crystal Structures", "MovingCrystalStructuresArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingCrystalStructuresArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Reference Good Features", "ReferenceGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getReferenceGoodFeaturesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Moving Good Features", "MovingGoodFeaturesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getMovingGoodFeaturesArrayPath(), false, ""));
    QStringList linkedProps;
    linkedProps<<"OutputFile";
  parameters.push_back(LinkedBooleanFilterParameter::New("Write Best Rotations to File", "WriteOutputs", getWriteOutputs(), linkedProps, false));
  parameters.push_back(FileSystemFilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget, getOutputFile(), false, "", "*.csv", "Comma Separated Data"));
  parameters.push_back(FilterParameter::New("Minimum Average Rotation Angle", "MinMiso", FilterParameterWidgetType::DoubleWidget, getMinMiso(), true, ""));
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
  setMovingGoodFeaturesArrayPath( reader->readDataArrayPath("MovingGoodFeaturesArrayPath", getMovingGoodFeaturesArrayPath() ) );
  setReferencePhasesArrayPath( reader->readDataArrayPath("ReferencePhasesArrayPath", getReferencePhasesArrayPath() ) );
  setMovingPhasesArrayPath( reader->readDataArrayPath("MovingPhasesArrayPath", getMovingPhasesArrayPath() ) );
  setReferenceCrystalStructuresArrayPath( reader->readDataArrayPath("ReferenceCrystalStructuresArrayPath", getReferenceCrystalStructuresArrayPath() ) );
  setMovingCrystalStructuresArrayPath( reader->readDataArrayPath("MovingCrystalStructuresArrayPath", getMovingCrystalStructuresArrayPath() ) );
  setWriteOutputs( reader->readValue("WriteOutputs", getWriteOutputs() ) );
  setOutputFile( reader->readString("OutputFile", getOutputFile() ) );
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
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingGoodFeaturesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferencePhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingPhasesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReferenceCrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(MovingCrystalStructuresArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(WriteOutputs)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputFile)
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

  //get pointers to prereq arrays
  QVector<size_t> dims(1, 4);
  m_ReferenceAvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getReferenceAvgQuatsArrayPath(), dims);
  m_MovingAvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getMovingAvgQuatsArrayPath(), dims);

  dims[0] = 1;
  m_ReferenceGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getReferenceGoodFeaturesArrayPath(), dims);
  m_MovingGoodFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getMovingGoodFeaturesArrayPath(), dims);

  m_ReferencePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferencePhasesArrayPath(), dims);
  m_MovingPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingPhasesArrayPath(), dims);

  m_ReferenceCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getReferenceCrystalStructuresArrayPath(), dims);
  m_MovingCrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getMovingCrystalStructuresArrayPath(), dims);

  //assign pointers
  if( NULL != m_ReferenceAvgQuatsPtr.lock().get() )
  {
    m_ReferenceAvgQuats = m_ReferenceAvgQuatsPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingAvgQuatsPtr.lock().get() )
  {
    m_MovingAvgQuats = m_MovingAvgQuatsPtr.lock()->getPointer(0);
  }
  if( NULL != m_ReferenceGoodFeaturesPtr.lock().get() )
  {
    m_ReferenceGoodFeatures = m_ReferenceGoodFeaturesPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingGoodFeaturesPtr.lock().get() )
  {
    m_MovingGoodFeatures = m_MovingGoodFeaturesPtr.lock()->getPointer(0);
  }
  if( NULL != m_ReferencePhasesPtr.lock().get() )
  {
    m_ReferencePhases = m_ReferencePhasesPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingPhasesPtr.lock().get() )
  {
    m_MovingPhases = m_MovingPhasesPtr.lock()->getPointer(0);
  }
  if( NULL != m_ReferenceCrystalStructuresPtr.lock().get() )
  {
    m_ReferenceCrystalStructures = m_ReferenceCrystalStructuresPtr.lock()->getPointer(0);
  }
  if( NULL != m_MovingCrystalStructuresPtr.lock().get() )
  {
    m_MovingCrystalStructures = m_MovingCrystalStructuresPtr.lock()->getPointer(0);
  }

  if(0 != getReferenceAvgQuatsArrayPath().getDataContainerName().compare(getReferencePhasesArrayPath().getDataContainerName()) || 0 != getReferenceAvgQuatsArrayPath().getAttributeMatrixName().compare(getReferencePhasesArrayPath().getAttributeMatrixName()))
  {
    QString ss = QObject::tr("The selected 'Reference Quats' and 'Reference Phases' must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, -1001);
    setErrorCondition(-1001);
    return;
  }
  if(0 != getReferenceAvgQuatsArrayPath().getDataContainerName().compare(getReferenceGoodFeaturesArrayPath().getDataContainerName()) || 0 != getReferenceAvgQuatsArrayPath().getAttributeMatrixName().compare(getReferenceGoodFeaturesArrayPath().getAttributeMatrixName()))
  {
    QString ss = QObject::tr("The selected 'Reference Quats' and 'Reference Good Features' must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, -1002);
    setErrorCondition(-1002);
    return;
  }
  if(0 != getMovingAvgQuatsArrayPath().getDataContainerName().compare(getMovingPhasesArrayPath().getDataContainerName()) || 0 != getMovingAvgQuatsArrayPath().getAttributeMatrixName().compare(getMovingPhasesArrayPath().getAttributeMatrixName()))
  {
    QString ss = QObject::tr("The selected 'Moving Quats' and 'Moving Phases' must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, -1003);
    setErrorCondition(-1003);
    return;
  }
  if(0 != getMovingAvgQuatsArrayPath().getDataContainerName().compare(getMovingGoodFeaturesArrayPath().getDataContainerName()) || 0 != getMovingAvgQuatsArrayPath().getAttributeMatrixName().compare(getMovingGoodFeaturesArrayPath().getAttributeMatrixName()))
  {
    QString ss = QObject::tr("The selected 'Moving Quats' and 'Moving Good Features' must belong to the same Data Container and Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, -1004);
    setErrorCondition(-1004);
    return;
  }
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
const QString RegisterOrientations::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getGroupName()
{
  return DatasetMerging::DatasetMergingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getHumanLabel()
{
  return "Register Orientations";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RegisterOrientations::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::AlignmentFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RegisterOrientations::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  //convert from float* to quat*
  QuatF* movingQuats = reinterpret_cast<QuatF*>(m_MovingAvgQuats);
  QuatF* referenceQuats = reinterpret_cast<QuatF*>(m_ReferenceAvgQuats);

  //create list of shared grains
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
        if(m_ReferenceCrystalStructures[m_ReferencePhases[i]] == m_MovingCrystalStructures[m_MovingPhases[i]])
        {
          sharedFeatures.push_back(i);
        }
      }
    }
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

  std::vector< QuatF > bestQuats(sharedFeatures.size());
  std::vector< float > avgMisorientations(sharedFeatures.size());

  //loop over matched features
  for(size_t i = 0; i<sharedFeatures.size(); i++)
  {
    //compute difference in orientation
    size_t index = sharedFeatures[i];
    uint32_t phase = m_ReferenceCrystalStructures[m_ReferencePhases[index]];//already checked that phases match

    QuatF gRefInv, gMov, tempQuat, candidateQuat;
    QuaternionMathF::Copy(referenceQuats[index], gRefInv);
    QuaternionMathF::Copy(movingQuats[index], gMov);
    QuaternionMathF::Conjugate(gRefInv);

    std::vector<QuatF> symops = symOpsList[phase];

    float minAvgMiso = 999.9f;
    QuatF bestQuat;

    //loop over symmetry operators for structure computing all possible equivilent ways of expressing rotation
    for(int j = 0; j < symops.size(); j++)
    {
      QuatF symQuat;
      QuaternionMathF::Copy(symops[j], symQuat);
      QuaternionMathF::Conjugate(symQuat);
      QuaternionMathF::Multiply(gMov, symQuat, tempQuat);
      QuaternionMathF::Multiply(tempQuat, gRefInv, candidateQuat);

      float avgMiso = 0.0f;
      int count = 0;

      //loop over all other pairs finding the resuling misorientation if the candidate rotation is applied to each
      for(size_t k = 0; k<sharedFeatures.size(); k++)
      {
        if(k != i)
        {
          QuatF otherGMovInv, otherGRef, otherGRefRot;
          size_t otherIndex = sharedFeatures[k];
          QuaternionMathF::Copy(referenceQuats[otherIndex], otherGRef);
          QuaternionMathF::Copy(movingQuats[otherIndex], otherGMovInv);
          QuaternionMathF::Conjugate(otherGMovInv);
          QuaternionMathF::Multiply(candidateQuat, otherGRef, otherGRefRot);

          float cos_min_angle = 0.0f;
          for(int l = 0; l < symops.size(); l++)
          {
            QuaternionMathF::Copy(symops[l], symQuat);
            QuaternionMathF::Multiply(otherGRefRot, symQuat, tempQuat);
            QuaternionMathF::Multiply(tempQuat, otherGMovInv, symQuat);
            QuaternionMathF::Copy(symQuat, tempQuat);

            if(tempQuat.w > cos_min_angle)
            {
              cos_min_angle = tempQuat.w;
            } else if( -tempQuat.w > cos_min_angle )
            {
              cos_min_angle = -tempQuat.w;
            }          
          }
          if(cos_min_angle>1) cos_min_angle = 1.0f;
          
          avgMiso+=acos(cos_min_angle);
          count++;
        }
      }

      avgMiso /= count;
      if(avgMiso<minAvgMiso)
      {
        minAvgMiso = avgMiso;
        QuaternionMathF::Copy(candidateQuat, bestQuat);
      }
    }

    //convert the average misorientation of the best match to degrees and add best match to list of matches
    if(bestQuat.w<0)//only consider rotations 0-180
    {
      QuaternionMathF::Negate(bestQuat);
    }
    QuaternionMathF::Copy(bestQuat, bestQuats[i]);
    avgMisorientations[i] = 2.0f*minAvgMiso;
  }

  //find average rotation
  double minMisoAngle = getMinMiso() * M_PI / 180.0f;
  QuatF rotation = QuaternionMathF::New(0,0,0,0);
  int count = 0;
  for(int i = 0; i<bestQuats.size(); i++)
  {
    if(avgMisorientations[i] < minMisoAngle)
    {
      QuaternionMathF::Add(rotation, bestQuats[i], rotation);
      count++;
    }
  }
  QuaternionMathF::ScalarDivide(rotation, count);

  //write infro to file if needed
  if(getWriteOutputs())
  {
    QFileInfo fi(getOutputFile());
    QDir parentPath(fi.path());
    if(!parentPath.mkpath("."))
    {
      QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath.absolutePath());
      notifyErrorMessage(getHumanLabel(), ss, -1);
      setErrorCondition(-1);
      return;
    }

    QFile file(getOutputFile());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QString ss = QObject::tr("Output file could not be opened: %1").arg(getOutputFile());
      setErrorCondition(-100);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QTextStream outFile(&file);
    outFile<<"applied rotation quat:, "<<rotation.x<<", "<<rotation.y<<", "<<rotation.z<<", "<<rotation.w<<", ("<<count<<" values averaged)\n";
    outFile<<"feature id, nx, ny, nz, w (radians), avg_miso (radians)\n";

    for(int i = 0; i<bestQuats.size(); i++)
    {
      float mag = sqrt(bestQuats[i].x*bestQuats[i].x + bestQuats[i].y*bestQuats[i].y + bestQuats[i].z*bestQuats[i].z);
      outFile<< sharedFeatures[i] <<",";
      outFile<< bestQuats[i].x / mag <<",";
      outFile<< bestQuats[i].y / mag <<",";
      outFile<< bestQuats[i].z / mag <<",";
      outFile<<2.0f * acos(bestQuats[i].w)<<",";
      outFile<<avgMisorientations[i]<<"\n";
    }
  }

  //apply rotation to align quats
  QuaternionMathF::Conjugate(rotation);
  for(int i=0; i<movingNumFeatures; i++)
  {
    QuatF origQuat, correctedQuat;
    QuaternionMathF::Copy(movingQuats[i], origQuat);
    QuaternionMathF::Multiply(rotation, origQuat, correctedQuat);
    QuaternionMathF::Copy(correctedQuat, movingQuats[i]);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RegisterOrientations::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  RegisterOrientations::Pointer filter = RegisterOrientations::New();
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

