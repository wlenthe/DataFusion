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
 
#include "RotateQuatReferenceFrame.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/DoubleFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

#include "SIMPLib/Math/QuaternionMath.hpp"

// Include the MOC generated file for this class
#include "moc_RotateQuatReferenceFrame.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateQuatReferenceFrame::RotateQuatReferenceFrame() :
  AbstractFilter(),
  m_QuatsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::FeatureData::AvgQuats),
  m_TransformPath(DREAM3D::Defaults::VolumeDataContainerName, DataFusionConstants::Transformation, DataFusionConstants::Transformation),
  m_Quats(NULL),
  m_TransformationType(1),
  m_Angle(0.0f)
{
  m_Axis.x = 0.0f;
  m_Axis.y = 0.0f;
  m_Axis.z = 1.0f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RotateQuatReferenceFrame::~RotateQuatReferenceFrame()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateQuatReferenceFrame::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::RequirementType req;
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Float, 4, DREAM3D::AttributeMatrixObjectType::Feature);
  parameters.push_back(DataArraySelectionFilterParameter::New("Quats", "QuatsArrayPath", getQuatsArrayPath(), FilterParameter::RequiredArray, req));
  {
    QVector<QString> choices;
      choices.push_back("Computed Value");
      choices.push_back("Manual Entry");
    QStringList linkedProps;
      linkedProps << "Axis" << "Angle" << "TransformPath";
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
      parameter->setHumanLabel("Transformation Type");
      parameter->setPropertyName("TransformationType");
      parameter->setChoices(choices);
      parameter->setLinkedProperties(linkedProps);
      parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(DataArraySelectionFilterParameter::New("Transformation", "TransformPath", getTransformPath(), FilterParameter::RequiredArray, req));
  parameters.back()->setGroupIndex(0);
  parameters.push_back(FloatVec3FilterParameter::New("Rotation Axis", "Axis", getAxis(), FilterParameter::Parameter));
  parameters.back()->setGroupIndex(1);
  parameters.push_back(DoubleFilterParameter::New("Rotation Angle (Degrees)", "Angle", getAngle(), FilterParameter::Parameter));
  parameters.back()->setGroupIndex(1);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateQuatReferenceFrame::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setQuatsArrayPath( reader->readDataArrayPath("QuatsArrayPath", getQuatsArrayPath() ) );
  setTransformationType( reader->readValue("TransformationType", getTransformationType() ) );
  setTransformPath( reader->readDataArrayPath("TransformPath", getTransformPath() ) );
  setAxis( reader->readFloatVec3("Axis", getAxis() ) );
  setAngle( reader->readValue("Angle", getAngle() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RotateQuatReferenceFrame::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(QuatsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(TransformationType)
  SIMPL_FILTER_WRITE_PARAMETER(TransformPath)
  SIMPL_FILTER_WRITE_PARAMETER(Axis)
  SIMPL_FILTER_WRITE_PARAMETER(Angle)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateQuatReferenceFrame::dataCheck()
{
  setErrorCondition(0);
  QVector<size_t> dims(1, 4);
  m_QuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getQuatsArrayPath(), dims);
  if( NULL != m_QuatsPtr.lock().get() ) m_Quats = m_QuatsPtr.lock()->getPointer(0);

  if(0 == getTransformationType()) {
    m_TransformPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getTransformPath(), dims);
    if( NULL != m_TransformPtr.lock().get() ) m_Transform = m_TransformPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RotateQuatReferenceFrame::preflight()
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
void RotateQuatReferenceFrame::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  QuatF rotation, qRot;
  QuatF* quats = reinterpret_cast<QuatF*>(m_Quats);
  size_t numQuats = m_QuatsPtr.lock()->getNumberOfTuples();

  switch(getTransformationType()) {
    case 0: {
      float mag = sqrt(m_Transform[0] * m_Transform[0] + m_Transform[1] * m_Transform[1] + m_Transform[2] * m_Transform[2]); 
      rotation.w = std::cos(m_Transform[3] / 2.0f);
      rotation.x = std::sin(m_Transform[3] / 2.0f) / mag;
      rotation.y = rotation.x;
      rotation.z = rotation.x;
      rotation.x *= m_Transform[0];
      rotation.y *= m_Transform[1];
      rotation.z *= m_Transform[2];
    } break;

    case 1: {
      float angle = getAngle() * std::acos(0) / 1800.0f;//angle/2
      float mag = std::sqrt(getAxis().x * getAxis().x + getAxis().y * getAxis().y + getAxis().z * getAxis().z);
      rotation.w = std::cos(angle);
      rotation.x = std::sin(angle) / mag;
      rotation.y = rotation.x;
      rotation.z = rotation.x;
      rotation.x *= getAxis().x;
      rotation.y *= getAxis().y;
      rotation.z *= getAxis().z;
    } break;
  }

  for(int i = 0; i < numQuats; i++) {
    QuaternionMathF::Multiply(quats[i], rotation, qRot);
    QuaternionMathF::Copy(qRot, quats[i]);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RotateQuatReferenceFrame::newFilterInstance(bool copyFilterParameters)
{
  RotateQuatReferenceFrame::Pointer filter = RotateQuatReferenceFrame::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateQuatReferenceFrame::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateQuatReferenceFrame::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateQuatReferenceFrame::getHumanLabel()
{ return "Rotate Quaternion Reference Frame"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RotateQuatReferenceFrame::getSubGroupName()
{ return DataFusionConstants::FilterGroups::DataFusionFilters; }

