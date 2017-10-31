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
 
#include "MatchFeatureIds.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

// Include the MOC generated file for this class
#include "moc_MatchFeatureIds.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIds::MatchFeatureIds() :
  AbstractFilter(),
  m_ReferenceCellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_MovingCellFeatureAttributeMatrixPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_ReferenceFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_MovingFeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_ReferenceUniqueArrayName(DataFusionConstants::UniqueFeatures),
  m_MovingUniqueArrayName(DataFusionConstants::UniqueFeatures),
  m_ReferenceUnique(NULL),
  m_MovingUnique(NULL),
  m_ReferenceFeatureIds(NULL),
  m_MovingFeatureIds(NULL)
{
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
  req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixObjectType::Element);
  parameters.push_back(DataArraySelectionFilterParameter::New("Reference Feature Ids", "ReferenceFeatureIdsArrayPath", getReferenceFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Moving Feature Ids", "MovingFeatureIdsArrayPath", getMovingFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));

  AttributeMatrixSelectionFilterParameter::RequirementType amReq;
  amReq = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixObjectType::Feature);
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Reference Cell Feature Attribute Matrix", "ReferenceCellFeatureAttributeMatrixPath", getReferenceCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, amReq));
  parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Moving Cell Feature Attribute Matrix", "MovingCellFeatureAttributeMatrixPath", getMovingCellFeatureAttributeMatrixPath(), FilterParameter::RequiredArray, amReq));

  parameters.push_back(StringFilterParameter::New("Reference Unique Grains", "ReferenceUniqueArrayName", getReferenceUniqueArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Moving Unique Grains", "MovingUniqueArrayName", getMovingUniqueArrayName(), FilterParameter::CreatedArray));
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

  setReferenceUniqueArrayName( reader->readString("ReferenceUniqueArrayName", getReferenceUniqueArrayName() ) );
  setMovingUniqueArrayName( reader->readString("MovingUniqueArrayName", getMovingUniqueArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchFeatureIds::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)

  SIMPL_FILTER_WRITE_PARAMETER(ReferenceFeatureIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(MovingFeatureIdsArrayPath)

  SIMPL_FILTER_WRITE_PARAMETER(ReferenceCellFeatureAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(MovingCellFeatureAttributeMatrixPath)

  SIMPL_FILTER_WRITE_PARAMETER(ReferenceUniqueArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(MovingUniqueArrayName)
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
    notifyErrorMessage(getHumanLabel(), "'Reference Cell Feature Attribute Matrix' must be of type Feature", -1000);
  if(DREAM3D::AttributeMatrixType::CellFeature !=moveCellFeatAttrMat->getType() )
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Feature Attribute Matrix' must be of type Feature", -1000);
  if(getReferenceCellFeatureAttributeMatrixPath() == getMovingCellFeatureAttributeMatrixPath())
    notifyErrorMessage(getHumanLabel(), "'Moving Cell Feature Attribute Matrix' and 'Reference Cell Feature Attribute Matrix' must be different", -1001);
  if(getErrorCondition() < 0) return;

  //required arrays
  QVector<size_t> dims(1, 1);
  m_ReferenceFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getReferenceFeatureIdsArrayPath(), dims);
  if( NULL != m_ReferenceFeatureIdsPtr.lock().get() )
  { m_ReferenceFeatureIds = m_ReferenceFeatureIdsPtr.lock()->getPointer(0); }

  m_MovingFeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getMovingFeatureIdsArrayPath(), dims);
  if( NULL != m_MovingFeatureIdsPtr.lock().get() )
  { m_MovingFeatureIds = m_MovingFeatureIdsPtr.lock()->getPointer(0); }

  if(getReferenceFeatureIdsArrayPath() == getMovingCellFeatureAttributeMatrixPath())
    notifyErrorMessage(getHumanLabel(), "'Moving Feature Ids' and 'Reference Feature Ids' must be different", -1001);
  if(getErrorCondition() < 0) return;

  //created arrays
  DataArrayPath tempPath;
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

std::vector<size_t> MatchFeatureIds::match() {
  //initially all grains are unmatched
  int maxMovingId = m_MovingUniquePtr.lock()->getNumberOfTuples();
  std::vector<size_t> idMap(maxMovingId, -1);
  idMap[0] = 0;
  return idMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIds::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) {return;}
  if(getCancel() == true) {return;}
  
  int maxReferenceId = m_ReferenceUniquePtr.lock()->getNumberOfTuples();
  int maxMovingId = m_MovingUniquePtr.lock()->getNumberOfTuples();

  //initially everything but grain 0 is unique (no grains have been matched)
  std::fill(m_ReferenceUnique, m_ReferenceUnique + maxReferenceId, true);
  std::fill(m_MovingUnique, m_MovingUnique + maxMovingId, true);
  m_ReferenceUnique[0] = false;
  m_MovingUnique[0] = false;

  // create map of current to new moving grain ids
  std::vector<size_t> idMap = match();

  for(size_t i = 1; i < maxMovingId; i++) {
    if(idMap[i] != static_cast<size_t>(-1)) {
      m_MovingUnique[i] = false;
      m_ReferenceUnique[idMap[i]] = false;
    }
  }

  //push unmatched moving grains above highest reference id
  int originalMovingSize = maxMovingId;
  maxMovingId = maxReferenceId;
  for(size_t i = 1; i < originalMovingSize; i++) {
    if(-1 == idMap[i]) {
      idMap[i] = maxMovingId++;
    }
  }

  //modify moving feature ids
  int64_t totalPoints = m_MovingFeatureIdsPtr.lock()->getNumberOfTuples();
  for(int i = 0; i < totalPoints; i++) {
    m_MovingFeatureIds[i] = idMap[m_MovingFeatureIds[i]];
  }

  //resize and rearrange moving feature attribute arrays
  QVector<size_t> tDims(1, maxMovingId);
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getMovingCellFeatureAttributeMatrixPath().getDataContainerName());
  AttributeMatrix::Pointer cellFeatureAttrMat = m->getAttributeMatrix(getMovingCellFeatureAttributeMatrixPath().getAttributeMatrixName());
  cellFeatureAttrMat->setTupleDimensions(tDims);

  QList<QString> featureArrayNames = cellFeatureAttrMat->getAttributeArrayNames();
  idMap.resize(tDims[0],0);
  QVector<size_t> newOrder = QVector<size_t>::fromStdVector(idMap);
  for(QList<QString>::iterator iter = featureArrayNames.begin(); iter != featureArrayNames.end(); ++iter) {
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

