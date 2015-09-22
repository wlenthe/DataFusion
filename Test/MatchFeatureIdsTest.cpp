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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "DataFusionTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  //no test files
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	QString filtName = "MatchFeatureIds";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The MatchFeatureIdsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatchFeatureIdsTest()
{
  //cell data
  size_t dims[] = {32};
  int32_t refID[] = {0, 0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 0};
  int32_t movID[] = {0, 8, 8, 7, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0};
  int32_t regID[] = {0, 1, 1, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 0};

  //feature data
  int32_t refSize[] = {0, 1, 2, 3, 4, 5, 6, 7, 1};
  int32_t movSize[] = {0, 7, 3, 5, 3, 6, 3, 1, 2};
  int32_t regSize[] = {0, 2, 1, 3, 6, 3, 5, 7, 0, 3};

  //create cell data
  QVector<size_t> tDims(1, dims[0]);
  QVector<size_t> cDims(1, 1);
  AttributeMatrix::Pointer cellAm = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
  DataArray<int32_t>::Pointer referenceIds = DataArray<int32_t>::CreateArray(tDims, cDims, "ReferenceFeatureIds");
  DataArray<int32_t>::Pointer movingIds = DataArray<int32_t>::CreateArray(tDims, cDims, "MovingFeatureIds");
  for(size_t i = 0; i < tDims[0]; i++) {
    referenceIds->setValue(i, refID[i]);
    movingIds->setValue(i, movID[i]);
  }
  cellAm->addAttributeArray(referenceIds->getName(), referenceIds);
  cellAm->addAttributeArray(movingIds->getName(), movingIds);

  //create cell feature data
  tDims[0] = 9;
  AttributeMatrix::Pointer refCellFeatAm = AttributeMatrix::New(tDims, "ReferenceCellFeatureData", DREAM3D::AttributeMatrixType::CellFeature);
  AttributeMatrix::Pointer movCellFeatAm = AttributeMatrix::New(tDims, "MovingCellFeatureData", DREAM3D::AttributeMatrixType::CellFeature);
  DataArray<int32_t>::Pointer referenceNumCells = DataArray<int32_t>::CreateArray(tDims, cDims, "ReferenceNumCells");
  DataArray<int32_t>::Pointer movingNumCells = DataArray<int32_t>::CreateArray(tDims, cDims, "MovingNumCells");
  for(size_t i = 0; i < tDims[0]; i++) {
    referenceNumCells->setValue(i, refSize[i]);
    movingNumCells->setValue(i, movSize[i]);
  }
  refCellFeatAm->addAttributeArray(referenceNumCells->getName(), referenceNumCells);
  movCellFeatAm->addAttributeArray(movingNumCells->getName(), movingNumCells);

  //fill a data container
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  image->setDimensions(dims);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer dc = DataContainer::New("dc");
  dc->setGeometry(image);
  dc->addAttributeMatrix(cellAm->getName(), cellAm);
  dc->addAttributeMatrix(refCellFeatAm->getName(), refCellFeatAm);
  dc->addAttributeMatrix(movCellFeatAm->getName(), movCellFeatAm);
  dca->addDataContainer(dc);

  //create match feature ids filter and execute
  QString filtName = "MatchFeatureIds";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if(NULL != filterFactory.get())
  {
    //create filter and set parameters
    AbstractFilter::Pointer filter = filterFactory->create();
    filter->setDataContainerArray(dca);

    QVariant var;
    bool propWasSet;
    DataArrayPath path;

    var.setValue(1);
    propWasSet = filter->setProperty("Metric", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(0.1);
    propWasSet = filter->setProperty("MetricThreshold", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);
    propWasSet = filter->setProperty("UseOrientations", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), cellAm->getName(), referenceIds->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceFeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), cellAm->getName(), movingIds->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("MovingFeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), refCellFeatAm->getName(), "");
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceCellFeatureAttributeMatrixPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), movCellFeatAm->getName(), "");
    var.setValue(path);
    propWasSet = filter->setProperty("MovingCellFeatureAttributeMatrixPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter and get output arrays
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    IDataArray::Pointer iFeatureIds = dc->getAttributeMatrix(cellAm->getName())->getAttributeArray(movingIds->getName());
    IDataArray::Pointer iNumCells = dc->getAttributeMatrix(movCellFeatAm->getName())->getAttributeArray(movingNumCells->getName());
    
    DataArray<int32_t>* pRegisteredIds = DataArray<int32_t>::SafePointerDownCast(iFeatureIds.get());
    DataArray<int32_t>* pRegisteredNumCells = DataArray<int32_t>::SafePointerDownCast(iNumCells.get());
    
    int32_t* registeredIds = pRegisteredIds->getPointer(0);
    int32_t* registeredNumCells = pRegisteredNumCells->getPointer(0);

    DREAM3D_REQUIRE_VALID_POINTER(registeredIds)
    DREAM3D_REQUIRE_VALID_POINTER(registeredNumCells)

    DREAM3D_REQUIRE_EQUAL(tDims[0] + 1, pRegisteredNumCells->getNumberOfTuples())

    for(size_t i = 0; i < dims[0]; i++) {
      DREAM3D_REQUIRE_EQUAL(registeredIds[i], regID[i])
    }
    for(size_t i = 0; i < pRegisteredNumCells->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(registeredNumCells[i], regSize[i])
    }
  } 
  else
  {
    QString ss = QObject::tr("MatchFeatureIdsTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
    DREAM3D_TEST_THROW_EXCEPTION(ss.toStdString())
  }

  return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loadFilterPlugins()
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();
}


// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("");
  QCoreApplication::setOrganizationDomain("");
  QCoreApplication::setApplicationName("MatchFeatureIdsTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( MatchFeatureIdsTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
