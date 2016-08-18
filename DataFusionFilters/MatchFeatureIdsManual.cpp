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
 
#include "MatchFeatureIdsManual.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DynamicTableFilterParameter.h"

#include "DataFusion/DataFusionConstants.h"

// Include the MOC generated file for this class
#include "moc_MatchFeatureIdsManual.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIdsManual::MatchFeatureIdsManual() :
  MatchFeatureIds()
{
  std::vector<std::vector <double> > emptyPairs(1, std::vector<double>(2, 0));
  DynamicTableData data = getManualPairs();
  data.setTableData(emptyPairs);
  setManualPairs(data);
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchFeatureIdsManual::~MatchFeatureIdsManual()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsManual::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  QStringList rowHeaders, columnHeaders;
  rowHeaders << "";
  columnHeaders << "Reference Id" << "Moving Id";
  parameters.push_back(DynamicTableFilterParameter::New("Pairs", "ManualPairs", rowHeaders, columnHeaders, getManualPairs().getTableData(), FilterParameter::Parameter, true, false, 1, 2));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsManual::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  MatchFeatureIds::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setManualPairs(reader->readDynamicTableData("ManualPairs", getManualPairs()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchFeatureIdsManual::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  MatchFeatureIds::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(ManualPairs)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsManual::dataCheck()
{
  setErrorCondition(0);
  MatchFeatureIds::dataCheck();
  std::vector< std::vector<double> > table = getManualPairs().getTableData();
  std::vector<int64_t> mov(table.size()), ref(table.size());
  for(size_t i = 0; i < table.size(); i++) {
    ref[i] = static_cast<int>(std::round(table[i][0]));
    mov[i] = static_cast<int>(std::round(table[i][1]));
    if(ref[i] <= 0 || mov[i] <= 0) {
      notifyErrorMessage(getHumanLabel(), "Manually paired feature ids must be positive.", -1);
      return;
    }
  }
  std::sort(mov.begin(), mov.end());
  std::sort(ref.begin(), ref.end());
  if((0 != std::distance(std::unique(mov.begin(), mov.end()), mov.end())) ||
     (0 != std::distance(std::unique(ref.begin(), ref.end()), ref.end())) )
      notifyErrorMessage(getHumanLabel(), "Features can only be paired once.", -1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsManual::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

std::vector<size_t> MatchFeatureIdsManual::match() {
  int maxReferenceId = getReferenceUniquePtr().lock()->getNumberOfTuples();
  int maxMovingId = getMovingUniquePtr().lock()->getNumberOfTuples();

  //initially everything but grain 0 is unique (no grains have been matched)
  std::fill(getReferenceUnique(), getReferenceUnique() + maxReferenceId, true);
  std::fill(getMovingUnique(), getMovingUnique() + maxMovingId, true);
  getReferenceUnique()[0] = false;
  getMovingUnique()[0] = false;

  //initially all grains are unmatched
  std::vector<size_t> idMap(maxMovingId, -1);
  idMap[0] = 0;

  //assign manual pairs
  std::vector< std::vector<double> > table = getManualPairs().getTableData();
  for(size_t i = 0; i < table.size(); i++) {
    idMap[static_cast<size_t>(std::round(table[i][1]))] = static_cast<size_t>(std::round(table[i][0]));
  }
  return idMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchFeatureIdsManual::execute()
{
  setErrorCondition(0);
  if(getErrorCondition() < 0) {return;}
  if(getCancel() == true) {return;}
  MatchFeatureIds::execute();
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchFeatureIdsManual::newFilterInstance(bool copyFilterParameters)
{
  MatchFeatureIdsManual::Pointer filter = MatchFeatureIdsManual::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsManual::getCompiledLibraryName()
{ return DataFusionConstants::DataFusionBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsManual::getGroupName()
{ return DREAM3D::FilterGroups::Unsupported; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsManual::getHumanLabel()
{ return "Match Feature Ids (Manual)"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchFeatureIdsManual::getSubGroupName()
{ return "DataFusion"; }

