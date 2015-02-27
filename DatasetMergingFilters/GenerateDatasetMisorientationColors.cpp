/*
 * Your License or Copyright Information can go here
 */

#include "GenerateDatasetMisorientationColors.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include <QtCore/QString>

#include "OrientationLib/Math/OrientationMath.h"
#include "DREAM3DLib/Utilities/ColorTable.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"

#include "DatasetMerging/DatasetMergingConstants.h"


class GenerateMisorientationColorsImpl
{

  public:
    GenerateMisorientationColorsImpl(QuatF* quats1, QuatF* quats2, int32_t* phases1, int32_t* phases2, unsigned int* crystalStructures1, unsigned int* crystalStructures2,
                                     bool* goodVoxels, uint8_t* notSupported, uint8_t* colors, float minAngle, bool* angleFlag) :
      m_Quats1(quats1),
      m_Quats2(quats2),
      m_CellPhases1(phases1),
      m_CellPhases2(phases2),
      m_CrystalStructures1(crystalStructures1),
      m_CrystalStructures2(crystalStructures2),
      m_GoodVoxels(goodVoxels),
      m_NotSupported(notSupported),
      m_MisorientationColor(colors),
      m_AngleTol(minAngle * DREAM3D::Constants::k_Pi / 180.0f),
      m_Flag(angleFlag)
    {}
    virtual ~GenerateMisorientationColorsImpl() {}

    void convert(size_t start, size_t end) const
    {
      std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();

      QuatF cellQuat1 = {0.0f, 0.0f, 0.0f, 1.0f};
      QuatF cellQuat2 = {0.0f, 0.0f, 0.0f, 1.0f};
      DREAM3D::Rgb argb = 0x00000000;

      bool missingGoodVoxels = false;
      if(NULL == m_GoodVoxels)
      {
        missingGoodVoxels = true;
      }
      int phase1 = 0;
      int phase2 = 0;
      size_t index = 0;
      for (size_t i = start; i < end; i++)
      {
        phase1 = m_CellPhases1[i];
        phase2 = m_CellPhases2[i];
        index = i * 3;
        m_MisorientationColor[index] = 0;
        m_MisorientationColor[index + 1] = 0;
        m_MisorientationColor[index + 2] = 0;
        m_Flag[i] = false;
        cellQuat1 = m_Quats1[i];
        cellQuat2 = m_Quats2[i];

        if(m_CrystalStructures1[phase1]!=m_CrystalStructures2[phase2])
        {
          m_NotSupported[13];
          m_MisorientationColor[index] = 0;
          m_MisorientationColor[index + 1] = 0;
          m_MisorientationColor[index + 2] = 0;
        }
        else if(m_CrystalStructures1[phase1] != Ebsd::CrystalStructure::Cubic_High)
        {
          uint32_t idx = m_CrystalStructures1[phase1];
          if(idx == Ebsd::CrystalStructure::UnknownCrystalStructure)
          {
            idx = 12;
          }
          m_NotSupported[idx] = 1;
          m_MisorientationColor[index] = 0;
          m_MisorientationColor[index + 1] = 0;
          m_MisorientationColor[index + 2] = 0;

        }
        // Make sure we are using a valid Euler Angles with valid crystal symmetry
        else if( (missingGoodVoxels == true || m_GoodVoxels[i] == true)
                 && m_CrystalStructures1[phase1] < Ebsd::CrystalStructure::LaueGroupEnd )
        {
          argb = ops[m_CrystalStructures1[phase1]]->generateMisorientationColor(cellQuat1, cellQuat2);
          m_MisorientationColor[index] = RgbColor::dRed(argb);
          m_MisorientationColor[index + 1] = RgbColor::dGreen(argb);
          m_MisorientationColor[index + 2] = RgbColor::dBlue(argb);

          //compute misorientation angle
          float n1, n2, n3;
          float angle = ops[m_CrystalStructures1[phase1]]->getMisoQuat(cellQuat1, cellQuat2, n1, n2, n3);
          if(angle > m_AngleTol)
          {
            m_Flag[i] = true;
          }
        }
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    QuatF* m_Quats1;
    QuatF* m_Quats2;
    int32_t* m_CellPhases1;
    int32_t* m_CellPhases2;
    unsigned int* m_CrystalStructures1;
    unsigned int* m_CrystalStructures2;
    bool* m_GoodVoxels;
    uint8_t* m_NotSupported;
    uint8_t* m_MisorientationColor;
    float m_AngleTol;
    bool* m_Flag;

};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateDatasetMisorientationColors::GenerateDatasetMisorientationColors() :
  AbstractFilter(),
  m_CellPhasesArrayPath1(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_CellPhasesArrayPath2(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
  m_QuatsArrayPath1(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_QuatsArrayPath2(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Quats),
  m_CrystalStructuresArrayPath1(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructuresArrayPath2(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::EnsembleData::CrystalStructures),
  m_GoodVoxelsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::GoodVoxels),
  m_MisorientationColorArrayName(DREAM3D::CellData::MisorientationColor),
  m_HighAngleFlagArrayName(DREAM3D::GeneralData::ThresholdArray),
  m_UseGoodVoxels(false),
  m_CellPhases1ArrayName(DREAM3D::CellData::Phases),
  m_CellPhases2ArrayName(DREAM3D::CellData::Phases),
  m_CellPhases1(NULL),
  m_CellPhases2(NULL),
  m_Quats1ArrayName(DREAM3D::CellData::Quats),
  m_Quats2ArrayName(DREAM3D::CellData::Quats),
  m_Quats1(NULL),
  m_Quats2(NULL),
  m_CrystalStructures1ArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures2ArrayName(DREAM3D::EnsembleData::CrystalStructures),
  m_CrystalStructures1(NULL),
  m_CrystalStructures2(NULL),
  m_MisorientationColor(NULL),
  m_HighAngleFlag(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL),
  m_MinAngle(5.0f)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateDatasetMisorientationColors::~GenerateDatasetMisorientationColors()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateDatasetMisorientationColors::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Low Angle Flag Tolerance", "MinAngle", FilterParameterWidgetType::DoubleWidget, getMinAngle(), false, ""));
  
  parameters.push_back(FilterParameter::New("Required Information (Dataset 1)", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath1", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath1(), false, ""));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath1", FilterParameterWidgetType::DataArraySelectionWidget, getQuatsArrayPath1(), false, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath1", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath1(), false, ""));
  
  parameters.push_back(FilterParameter::New("Required Information (Dataset 2)", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Cell Phases", "CellPhasesArrayPath2", FilterParameterWidgetType::DataArraySelectionWidget, getCellPhasesArrayPath2(), false, ""));
  parameters.push_back(FilterParameter::New("Quats", "QuatsArrayPath2", FilterParameterWidgetType::DataArraySelectionWidget, getQuatsArrayPath2(), false, ""));
  parameters.push_back(FilterParameter::New("Crystal Structures", "CrystalStructuresArrayPath2", FilterParameterWidgetType::DataArraySelectionWidget, getCrystalStructuresArrayPath2(), false, ""));
  
  parameters.push_back(FilterParameter::New("Optional Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  QStringList linkedProps("GoodVoxelsArrayPath");
  parameters.push_back(LinkedBooleanFilterParameter::New("Apply to Good Voxels Only (Bad Voxels Will Be Black)", "UseGoodVoxels", getUseGoodVoxels(), linkedProps, true));
  parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGoodVoxelsArrayPath(), true, ""));

  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("MisorientationColor", "MisorientationColorArrayName", FilterParameterWidgetType::StringWidget, getMisorientationColorArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("HighAngleFlag", "HighAngleFlagArrayName", FilterParameterWidgetType::StringWidget, getHighAngleFlagArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateDatasetMisorientationColors::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMinAngle(reader->readValue("MinAngle", getMinAngle() ) );
  setUseGoodVoxels(reader->readValue("UseGoodVoxels", getUseGoodVoxels() ) );
  setMisorientationColorArrayName(reader->readString("MisorientationColorArrayName", getMisorientationColorArrayName() ) );
  setHighAngleFlagArrayName(reader->readString("HighAngleFlagArrayName", getHighAngleFlagArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCrystalStructuresArrayPath1(reader->readDataArrayPath("CrystalStructuresArrayPath1", getCrystalStructuresArrayPath1() ) );
  setCrystalStructuresArrayPath2(reader->readDataArrayPath("CrystalStructuresArrayPath2", getCrystalStructuresArrayPath2() ) );
  setQuatsArrayPath1(reader->readDataArrayPath("QuatsArrayPath1", getQuatsArrayPath1() ) );
  setQuatsArrayPath2(reader->readDataArrayPath("QuatsArrayPath2", getQuatsArrayPath2() ) );
  setCellPhasesArrayPath1(reader->readDataArrayPath("CellPhasesArrayPath1", getCellPhasesArrayPath1() ) );
  setCellPhasesArrayPath2(reader->readDataArrayPath("CellPhasesArrayPath2", getCellPhasesArrayPath2() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenerateDatasetMisorientationColors::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(MinAngle)
  DREAM3D_FILTER_WRITE_PARAMETER(UseGoodVoxels)
  DREAM3D_FILTER_WRITE_PARAMETER(MisorientationColorArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(HighAngleFlagArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(GoodVoxelsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath1)
  DREAM3D_FILTER_WRITE_PARAMETER(CrystalStructuresArrayPath2)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath1)
  DREAM3D_FILTER_WRITE_PARAMETER(QuatsArrayPath2)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath1)
  DREAM3D_FILTER_WRITE_PARAMETER(CellPhasesArrayPath2)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateDatasetMisorientationColors::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  //make sure arrays are from same attribute matrix
  if(0!=getCellPhasesArrayPath1().getAttributeMatrixName().compare(getCellPhasesArrayPath1().getAttributeMatrixName()))
  {
    setErrorCondition(-1);
    notifyWarningMessage(getHumanLabel(), "The selected phase arrays must be from the same attribute matrix", getErrorCondition());
    return;
  }
  if(0!=getQuatsArrayPath1().getAttributeMatrixName().compare(getQuatsArrayPath2().getAttributeMatrixName()))
  {
    setErrorCondition(-2);
    notifyWarningMessage(getHumanLabel(), "The selected quat arrays must be from the same attribute matrix", getErrorCondition());
    return;
  }

  QVector<size_t> dims(1, 1);
  m_CellPhases1Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath1(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhases1Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases1 = m_CellPhases1Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPhases2Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath2(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPhases2Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPhases2 = m_CellPhases2Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 4;
  m_Quats1Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath1(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Quats1Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats1 = m_Quats1Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_Quats2Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath2(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_Quats2Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Quats2 = m_Quats2Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  typedef DataArray<unsigned int> XTalStructArrayType;
  dims[0] = 1;
  m_CrystalStructures1Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath1(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructures1Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures1 = m_CrystalStructures1Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CrystalStructures2Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath2(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CrystalStructures2Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CrystalStructures2 = m_CrystalStructures2Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  
  tempPath.update(m_CellPhasesArrayPath1.getDataContainerName(), m_CellPhasesArrayPath1.getAttributeMatrixName(), getHighAngleFlagArrayName() );
  m_HighAngleFlagPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_HighAngleFlagPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_HighAngleFlag = m_HighAngleFlagPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0] = 3;
  tempPath.update(m_CellPhasesArrayPath1.getDataContainerName(), m_CellPhasesArrayPath1.getAttributeMatrixName(), getMisorientationColorArrayName() );
  m_MisorientationColorPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_MisorientationColorPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_MisorientationColor = m_MisorientationColorPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */



  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  if (getUseGoodVoxels() == true)
  {
    // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
    dims[0] = 1;
    m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    m_GoodVoxels = NULL;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateDatasetMisorientationColors::preflight()
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
const QString GenerateDatasetMisorientationColors::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateDatasetMisorientationColors::getGroupName()
{
  return DatasetMerging::DatasetMergingPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateDatasetMisorientationColors::getHumanLabel()
{
  return "Generate Dataset Misorientation Colors";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateDatasetMisorientationColors::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateDatasetMisorientationColors::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);


  int64_t totalPoints = m_CellPhases1Ptr.lock()->getNumberOfTuples();

  bool missingGoodVoxels = true;
  if (NULL != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }

  // Create 1 of every type of Ops class. This condenses the code below
  UInt8ArrayType::Pointer notSupported = UInt8ArrayType::CreateArray(14, "NotSupportedArray");
  notSupported->initializeWithZeros();

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      GenerateMisorientationColorsImpl(reinterpret_cast<QuatF*>(m_Quats1), reinterpret_cast<QuatF*>(m_Quats2), m_CellPhases1, m_CellPhases2, m_CrystalStructures1, m_CrystalStructures2, m_GoodVoxels, notSupported->getPointer(0), m_MisorientationColor, m_MinAngle, m_HighAngleFlag), tbb::auto_partitioner());

  }
  else
#endif
  {
    GenerateMisorientationColorsImpl serial(reinterpret_cast<QuatF*>(m_Quats1), reinterpret_cast<QuatF*>(m_Quats2), m_CellPhases1, m_CellPhases2, m_CrystalStructures1, m_CrystalStructures2, m_GoodVoxels, notSupported->getPointer(0), m_MisorientationColor, m_MinAngle, m_HighAngleFlag);
    serial.convert(0, totalPoints);
  }

  std::vector<OrientationOps::Pointer> ops = OrientationOps::getOrientationOpsVector();

  // Check and warn about unsupported crystal symmetries in the computation which will show as black
  for(size_t i = 0; i < notSupported->getNumberOfTuples() - 2; i++)
  {
    if (notSupported->getValue(i) == 1)
    {
      QString msg("The Symmetry of ");
      msg.append(ops[i]->getSymmetryName()).append(" is not currently supported for Misorientation Coloring. Voxels with this symmetry have been set to black.");
      notifyWarningMessage(getHumanLabel(), msg, -500);
    }
  }

  // Check for bad voxels which will show up as black also.
  if (notSupported->getValue(12) == 1)
  {
    QString msg("There were voxels with an unknown crystal symmetry due most likely being marked as a 'Bad Voxel'. These voxels have been colored black BUT black is a valid color for Misorientation coloring. Please understand this when visualizing your data.");
    notifyWarningMessage(getHumanLabel(), msg, -501);
  }

  // Check for mixed voxels which will show up as black also.
  if (notSupported->getValue(13) == 1)
  {
    QString msg("There were voxels with 2 different phases. These voxels have been colored black BUT black is a valid color for Misorientation coloring. Please understand this when visualizing your data.");
    notifyWarningMessage(getHumanLabel(), msg, -501);
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
AbstractFilter::Pointer GenerateDatasetMisorientationColors::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  GenerateDatasetMisorientationColors::Pointer filter = GenerateDatasetMisorientationColors::New();
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

