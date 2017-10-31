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
#include "SIMPLib/Geometry/ImageGeom.h"

#include "DataFusionTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	QString filtName = "RenumberFeatures";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The RenumberFeaturesTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenumberFeaturesTest()
{
  //cell data
  size_t dims[] = {32};
  int32_t refID[] = {0, 0, 5, 1, 1, 3, 3, 3, 4, 4, 4, 4, 7, 7, 7, 7, 7, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 0, 0};
  int32_t decId[] = {0, 0, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0};
  int32_t ascId[] = {0, 0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 0, 0};

  //feature data
  int32_t refSizes[] = {0, 2, 6, 3, 4, 1, 7, 5};
  int32_t decSizes[] = {0, 7, 6, 5, 4, 3, 2, 1};
  int32_t ascSizes[] = {0, 1, 2, 3, 4, 5, 6, 7};

  //create cell data
  QVector<size_t> tDims(1, dims[0]);
  QVector<size_t> cDims(1, 1);
  AttributeMatrix::Pointer cellAm = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::AttributeMatrixType::Cell);
  DataArray<int32_t>::Pointer featureIds = DataArray<int32_t>::CreateArray(tDims, cDims, "FeatureIds");
  for(size_t i = 0; i < tDims[0]; i++) {
    featureIds->setValue(i, refID[i]);
  }
  cellAm->addAttributeArray(featureIds->getName(), featureIds);

  //create cell feature data
  tDims[0] = 8;
  AttributeMatrix::Pointer featAm = AttributeMatrix::New(tDims, "CellFeatureData", DREAM3D::AttributeMatrixType::CellFeature);
  DataArray<int32_t>::Pointer numCells = DataArray<int32_t>::CreateArray(tDims, cDims, "NumCells");
  for(size_t i = 0; i < tDims[0]; i++) {
    numCells->setValue(i, refSizes[i]);
  }
  featAm->addAttributeArray(numCells->getName(), numCells);

  //fill a data container
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  image->setDimensions(dims);

  DataContainer::Pointer dc = DataContainer::New("dc");
  dc->setGeometry(image);
  dc->addAttributeMatrix(cellAm->getName(), cellAm);
  dc->addAttributeMatrix(featAm->getName(), featAm);

  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(dc);

  //create filter, execute, and check results
  QString filtName = "RenumberFeatures";
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

    var.setValue(0);//0: Descending, 1: Ascending
    propWasSet = filter->setProperty("Order", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), cellAm->getName(), featureIds->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("FeatureIdsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), featAm->getName(), numCells->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ScalarArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter and get output arrays
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    IDataArray::Pointer iFeatureIds = dc->getAttributeMatrix(cellAm->getName())->getAttributeArray(featureIds->getName());
    IDataArray::Pointer iNumCells = dc->getAttributeMatrix(featAm->getName())->getAttributeArray(numCells->getName());
    
    DataArray<int32_t>* pFeatureIds = DataArray<int32_t>::SafePointerDownCast(iFeatureIds.get());
    DataArray<int32_t>* pNumCells = DataArray<int32_t>::SafePointerDownCast(iNumCells.get());
    
    int32_t* fids = pFeatureIds->getPointer(0);
    int32_t* nc = pNumCells->getPointer(0);

    DREAM3D_REQUIRE_VALID_POINTER(fids)
    DREAM3D_REQUIRE_VALID_POINTER(nc)

    for(size_t i = 0; i < pFeatureIds->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(fids[i], decId[i])
    }
    for(size_t i = 0; i < pNumCells->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(nc[i], decSizes[i])
    }

    //rerun with ascending sort
    var.setValue(1);//0: Descending, 1: Ascending
    propWasSet = filter->setProperty("Order", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    iFeatureIds = dc->getAttributeMatrix(cellAm->getName())->getAttributeArray(featureIds->getName());
    iNumCells = dc->getAttributeMatrix(featAm->getName())->getAttributeArray(numCells->getName());
    
    pFeatureIds = DataArray<int32_t>::SafePointerDownCast(iFeatureIds.get());
    pNumCells = DataArray<int32_t>::SafePointerDownCast(iNumCells.get());
    
    fids = pFeatureIds->getPointer(0);
    nc = pNumCells->getPointer(0);

    DREAM3D_REQUIRE_VALID_POINTER(fids)
    DREAM3D_REQUIRE_VALID_POINTER(nc)

    for(size_t i = 0; i < pFeatureIds->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(fids[i], ascId[i])
    }
    for(size_t i = 0; i < pNumCells->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(nc[i], ascSizes[i])
    }
  } 
  else
  {
    QString ss = QObject::tr("RenumberFeaturesTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
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
  QCoreApplication::setApplicationName("RenumberFeaturesTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( RenumberFeaturesTest() )

  PRINT_TEST_SUMMARY();
  return err;
}
