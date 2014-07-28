/*
 * Your License or Copyright Information can go here
 */

#include "MergeDatasets.h"

#include <QtCore/QString>

#include "DatasetMerging/DatasetMergingConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeDatasets::MergeDatasets() :
  AbstractFilter(),
  m_UseTranslation(true),
  m_UseRotation(false),
  m_RotationAngle(0),
  m_UseScaling(false),
  m_UseShearing(false)
/* DO NOT FORGET TO INITIALIZE ALL YOUR DREAM3D Filter Parameters HERE */
{
  m_TranslationParameters.x = 0.0f;
  m_TranslationParameters.y = 0.0f;
  m_TranslationParameters.z = 0.0f;

  m_RotationAxis.x = 0.0f;
  m_RotationAxis.y = 0.0f;
  m_RotationAxis.z = 1.0f;

  m_ScalingParamters.x = 100.0f;
  m_ScalingParamters.y = 100.0f;
  m_ScalingParamters.z = 100.0f;

  m_ShearingParameters.x = 0.0f;
  m_ShearingParameters.y = 0.0f;
  m_ShearingParameters.z = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeDatasets::~MergeDatasets()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeDatasets::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FilterParameter::New("Allowed Transformations (Degrees of Freedom)", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  QStringList rigidLinkedProps("TranslationParameters");
  parameters.push_back(FilterParameter::NewConditional("Translation", "UseTranslation", FilterParameterWidgetType::LinkedBooleanWidget, getUseTranslation(), false, rigidLinkedProps));
  parameters.push_back(FilterParameter::New("Translation Amounts", "TranslationParameters", FilterParameterWidgetType::FloatVec3Widget, getTranslationParameters(), false));

  QStringList rotationLinkedProps;
  rotationLinkedProps<<"RotationAxis"<<"RotationAngle";
  parameters.push_back(FilterParameter::NewConditional("Rotation", "UseRotation", FilterParameterWidgetType::LinkedBooleanWidget, getUseRotation(), false, rotationLinkedProps));
  parameters.push_back(FilterParameter::New("Rotation Axis", "RotationAxis", FilterParameterWidgetType::FloatVec3Widget, getRotationAxis(), false));
  parameters.push_back(FilterParameter::New("Rotation Angle", "RotationAngle", FilterParameterWidgetType::DoubleWidget, getRotationAngle(), false));

  QStringList scalingLinkedProps("ScalingParamters");
  // parameters.push_back(FilterParameter::NewConditional("Similarity", "UseGoodVoxels", FilterParameterWidgetType::LinkedBooleanWidget, getUseGoodVoxels(), false, linkedProps));
  parameters.push_back(FilterParameter::NewConditional("Scaling", "UseScaling", FilterParameterWidgetType::LinkedBooleanWidget, getUseScaling(), false, scalingLinkedProps));
  parameters.push_back(FilterParameter::New("Scaling Amounts", "ScalingParamters", FilterParameterWidgetType::FloatVec3Widget, getScalingParamters(), false));

  QStringList shearingLinkedProps("ShearingParameters");
  parameters.push_back(FilterParameter::NewConditional("Shearing", "UseShearing", FilterParameterWidgetType::LinkedBooleanWidget, getUseShearing(), false, shearingLinkedProps));
  parameters.push_back(FilterParameter::New("Shearing Amounts", "ShearingParameters", FilterParameterWidgetType::FloatVec3Widget, getShearingParameters(), false));
  
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeDatasets::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);

  setUseTranslation(reader->readValue("UseTranslation", getUseTranslation() ) );
  setTranslationParameters( reader->readFloatVec3("TranslationParameters", getTranslationParameters() ) );

  setUseRotation(reader->readValue("UseRotation", getUseRotation() ) );
  setRotationAxis( reader->readFloatVec3("RotationAxis", getRotationAxis() ) );
  setRotationAngle( reader->readValue("RotationAngle", getRotationAngle() ) );

  setUseScaling(reader->readValue("UseScaling", getUseScaling() ) );
  setScalingParamters( reader->readFloatVec3("ScalingParamters", getScalingParamters() ) );

  setUseShearing(reader->readValue("UseShearing", getUseShearing() ) );
  setShearingParameters( reader->readFloatVec3("ShearingParameters", getShearingParameters() ) );

  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeDatasets::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(UseTranslation)
  DREAM3D_FILTER_WRITE_PARAMETER(TranslationParameters)
  DREAM3D_FILTER_WRITE_PARAMETER(UseRotation)
  DREAM3D_FILTER_WRITE_PARAMETER(RotationAxis)
  DREAM3D_FILTER_WRITE_PARAMETER(RotationAngle)
  DREAM3D_FILTER_WRITE_PARAMETER(UseScaling)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalingParamters)
  DREAM3D_FILTER_WRITE_PARAMETER(UseShearing)
  DREAM3D_FILTER_WRITE_PARAMETER(ShearingParameters)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeDatasets::dataCheck()
{
  setErrorCondition(0);

  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.empty() == true)
  {
    QString ss = QObject::tr("Output file name was not set").arg(getHumanLabel());
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
    return;
  }
  * We can also check for the availability of REQUIRED ARRAYS:
  * QVector<size_t> dims(1, 1);
  * // Assigns the shared_ptr<> to an instance variable that is a weak_ptr<>
  * m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), dims);
  *  // Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object
  * if( NULL != m_CellPhasesPtr.lock().get() )
  * {
  *   // Now assign the raw pointer to data from the DataArray<T> object
  *   m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  * }
  *
  * We can also CREATE a new array to dump new data into
  *   tempPath.update(m_CellEulerAnglesArrayPath.getDataContainerName(), m_CellEulerAnglesArrayPath.getAttributeMatrixName(), getCellIPFColorsArrayName() );
  * // Assigns the shared_ptr<> to an instance variable that is a weak_ptr<>
  * m_CellIPFColorsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, dims);
  * // Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object
  * if( NULL != m_CellIPFColorsPtr.lock().get() )
  * {
  * // Now assign the raw pointer to data from the DataArray<T> object
  * m_CellIPFColors = m_CellIPFColorsPtr.lock()->getPointer(0);
  * }
  */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeDatasets::preflight()
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
const QString MergeDatasets::getCompiledLibraryName()
{
  return DatasetMerging::DatasetMergingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeDatasets::getGroupName()
{
  return "DatasetMerging";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeDatasets::getHumanLabel()
{
  return "MergeDatasets";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeDatasets::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeDatasets::execute()
{
  int err = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

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
AbstractFilter::Pointer MergeDatasets::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  MergeDatasets::Pointer filter = MergeDatasets::New();
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

