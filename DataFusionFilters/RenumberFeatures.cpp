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

#include "RenumberFeatures.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeatures::RenumberFeatures() :
  AbstractFilter(),
  m_FeatureIds(NULL),
  m_FeatureIdsArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
  m_ScalarArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_Order(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenumberFeatures::~RenumberFeatures()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::DataStructureRequirements req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
  parameters.push_back(DataArraySelectionFilterParameter::New("Scalar Array", "ScalarArrayPath", getScalarArrayPath(), FilterParameter::RequiredArray, req));

  QVector<QString> choices;
    choices.push_back("Descending");
    choices.push_back("Ascending");
  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Order");
    parameter->setPropertyName("Order");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
  parameters.push_back(parameter);
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath( reader->readDataArrayPath( "FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
  setScalarArrayPath( reader->readDataArrayPath( "ScalarArrayPath", getScalarArrayPath() ) );
  setOrder( reader->readValue("Order", getOrder()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalarArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Order)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::dataCheck()
{
  setErrorCondition(0);

  //required arrays
  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims);
  if( NULL != m_FeatureIdsPtr.lock().get() )
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); }


  if(!m_ScalarArrayPath.isValid())
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the DataContainer must be selected.", getErrorCondition());
  }
  else
  {
    IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_ScalarArrayPath.getDataContainerName())->getAttributeMatrix(m_ScalarArrayPath.getAttributeMatrixName())->getAttributeArray(m_ScalarArrayPath.getDataArrayName());
    if (NULL == inputData.get())
    {
      setErrorCondition(-11001);
      QString ss = QObject::tr("Data array '%1' does not exist in the DataContainer.").arg(m_ScalarArrayPath.getDataArrayName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else
    {
      if(1 != inputData->getNumberOfComponents())
      {
        setErrorCondition(-11002);
        QString ss = QObject::tr("Data Array '%1' must be a scalar array").arg(m_ScalarArrayPath.getDataArrayName());
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
    }
  }
  
  if(getErrorCondition() < 0) return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

template <typename T>
class RenumberFeaturesCompare {
  public:
    RenumberFeaturesCompare(IDataArray::Pointer pSort, int order) : m_ascending(1 == order) {m_sortArray = DataArray<T>::SafePointerDownCast(pSort.get())->getPointer(0);}
    bool operator()(const size_t& i, const size_t& j) const {return m_ascending ? m_sortArray[i] < m_sortArray[j] : m_sortArray[i] > m_sortArray[j];}

  private: 
    const T* m_sortArray;
    const bool m_ascending;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenumberFeatures::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getCancel() == true) { return; }

  //get number of features
  IDataArray::Pointer p = getDataContainerArray()->getDataContainer(m_ScalarArrayPath.getDataContainerName())->getAttributeMatrix(m_ScalarArrayPath.getAttributeMatrixName())->getAttributeArray(m_ScalarArrayPath.getDataArrayName());
  size_t numFeatures = p->getNumberOfTuples();

  //create map of old->new feature ids (initially maps to self)
  std::vector<size_t> fromIds(numFeatures);
  for(size_t i = 0; i < numFeatures; i++)
    fromIds[i] = i;

  //sort map by scalar array
  QString typeName = p->getTypeAsString();
  if (typeName.compare("int8_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<int8_t>(p, m_Order));     
  } else if (typeName.compare("uint8_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<uint8_t>(p, m_Order));     
  } else if (typeName.compare("int16_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<int16_t>(p, m_Order));     
  } else if (typeName.compare("uint16_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<uint16_t>(p, m_Order));     
  } else if (typeName.compare("int32_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<int32_t>(p, m_Order));     
  } else if (typeName.compare("uint32_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<uint32_t>(p, m_Order));     
  } else if (typeName.compare("int64_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<int64_t>(p, m_Order));     
  } else if (typeName.compare("uint64_t") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<uint64_t>(p, m_Order));     
  } else if (typeName.compare("float") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<float>(p, m_Order));     
  } else if (typeName.compare("double") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<double>(p, m_Order));     
  } else if (typeName.compare("bool") == 0) {
    std::sort(fromIds.begin() + 1, fromIds.end(), RenumberFeaturesCompare<bool>(p, m_Order));     
  } else {
    setErrorCondition(-101);
    QString ss = QObject::tr("Data array '%1' is of unsupported type '%2'.").arg(m_ScalarArrayPath.getDataArrayName()).arg(typeName);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  //reverse map (we have oldID[newId] but need newId[oldId])
  std::vector<size_t> toIds(numFeatures);
  for(size_t i = 0; i < numFeatures; i++)
    toIds[fromIds[i]] = i;

  //renumber feature ids
  int64_t numCells = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(int64_t i = 0; i < numCells; i++)
    m_FeatureIds[i] = toIds[m_FeatureIds[i]];

  //reorder arrays in feature attribute matrix
  AttributeMatrix::Pointer featureAttrMat = getDataContainerArray()->getDataContainer(getScalarArrayPath().getDataContainerName())->getAttributeMatrix(getScalarArrayPath().getAttributeMatrixName());
  QList<QString> arrayNames = featureAttrMat->getAttributeArrayNames();
  QVector<size_t> newOrder = QVector<size_t>::fromStdVector(toIds);
  for(QList<QString>::iterator iter = arrayNames.begin(); iter != arrayNames.end(); ++iter)
  {
    IDataArray::Pointer pOld = featureAttrMat->getAttributeArray(*iter);
    IDataArray::Pointer pNew = pOld->reorderCopy(newOrder);
    featureAttrMat->removeAttributeArray(*iter);
    featureAttrMat->addAttributeArray(pNew->getName(), pNew);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenumberFeatures::newFilterInstance(bool copyFilterParameters)
{
  RenumberFeatures::Pointer filter = RenumberFeatures::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeatures::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeatures::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeatures::getHumanLabel()
{ return "Renumber Features"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenumberFeatures::getSubGroupName()
{ return DataFusionConstants::FilterGroups::DataFusionFilters; }

