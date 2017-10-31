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

#include "SIMPLib/FilterParameters/DynamicTableData.h"

#include "DataFusionTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the FuseVolumesTest Filter from the FilterManager
	QString filtName = "FuseVolumes";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The FuseVolumesTestTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FuseVolumesTestTest()
{
  //test procedure:
  //-create 2 identical volumes with different resolutions / orientations
  //  -coarse dataset is single voxel features, increasing in index order (x fastest, z slowest), with a single voxel thick 0 pad on all sides
  //  -fine dataset is the same as coarse dataset with higher resolution and rotated 180 @ 001
  //-fuse with higher resolution dataset as reference
  //-compare all voxels

  static const size_t cX = 6, cY = 5, cZ = 4;//coarse dataset dimensions (excluding padding)
  static const size_t sX = 3, sY = 2, sZ = 1;//size of features in fine dataset (e.g. 1, 2, 3 -> each voxel in the coarse volume is 6 in the fine volume (split in half in y and thirds in z))

  //compute fine dataset dimensions
  static const size_t fX = cX * sX;
  static const size_t fY = cY * sY;
  static const size_t fZ = cZ * sZ;

  //create arrays to hold both feature ids
  QVector<size_t> coarseDims(3), fineDims(3);
  float fineRes[3] = {1.0 / static_cast<double>(sX), 1.0 / static_cast<double>(sY), 1.0 / static_cast<double>(sZ)};
  float fineOrig[3] = {-fineRes[0], -fineRes[1], -fineRes[2]};//origin such that first nonzero voxel is at (0, 0, 0)
  float coarseOrig[3] = {-1.0f, -1.0f, -1.0f};//origin such that first nonzero voxel is at (0, 0, 0)
  coarseDims[0] = cX + 2;
  coarseDims[1] = cY + 2;
  coarseDims[2] = cZ + 2;
  fineDims[0] = fX + 2;
  fineDims[1] = fY + 2;
  fineDims[2] = fZ + 2;
  QVector<size_t> cDims(1, 1);
  DataArray<int32_t>::Pointer pCoarseIds = DataArray<int32_t>::CreateArray(coarseDims, cDims, "FeatureIds");
  DataArray<int32_t>::Pointer pFineIds = DataArray<int32_t>::CreateArray(fineDims, cDims, "FeatureIds");
  pCoarseIds->initializeWithValue(0);
  pFineIds->initializeWithValue(0);
  int32_t* coarseIds = pCoarseIds->getPointer(0);
  int32_t* fineIds = pFineIds->getPointer(0);

  //fill coarse dataset
  size_t index = 0;
  for(size_t k = 0; k < cZ; k++) {
    size_t zOffset = (k + 1) * (cX + 2) * (cY + 2);
    for(size_t j = cY; j > 0; j--) {
      size_t yOffset = j * (cX + 2);
      for(size_t i = cX; i >= 1; i--) {
        coarseIds[zOffset + yOffset + i] = ++index;
      }
    }
  }

  //fill fine dataset
  index = 0;
  for(size_t k = 0; k < fZ; k += sZ) {
    size_t zOffset = (k + 1) * (fX + 2) * (fY + 2);
    for(size_t j = 0; j < fY; j += sY) {
      size_t yOffset = (j + 1) * (fX + 2);
      for(size_t i = 0; i < fX; i += sX) {
        //seed each feature in bottom corner and expand across x
        size_t offset = zOffset + yOffset + + i + 1;
        fineIds[offset] = ++index;
        for(size_t m = 0; m < sX; m++) {
          fineIds[offset + m] = fineIds[offset];
        }
      }
      //expand features across y by copying rows
      size_t offset = zOffset + yOffset;
      for(size_t m = 1; m < sY; m++) {
        std::memcpy(&fineIds[offset + m * (fX + 2)], &fineIds[offset], (fX + 2) * sizeof(int32_t));
      }
    }
    //expand features across z by copying slices
    for(size_t m = 0; m < sZ; m++) {
      std::memcpy(&fineIds[zOffset + m * (fX + 2) * (fY + 2)], &fineIds[zOffset], (fX + 2) * (fY + 2) * sizeof(int32_t));
    }
  }

  //create attribute matrix to hold each set of feature ids and fill
  AttributeMatrix::Pointer refAm = AttributeMatrix::New(fineDims, "ReferenceCellData", DREAM3D::AttributeMatrixType::Cell);
  AttributeMatrix::Pointer movAm = AttributeMatrix::New(coarseDims, "MovingCellData", DREAM3D::AttributeMatrixType::Cell);
  refAm->addAttributeArray(pFineIds->getName(), pFineIds);
  movAm->addAttributeArray(pCoarseIds->getName(), pCoarseIds);

  //create data container for each attribute matrix and fill
  ImageGeom::Pointer fImage = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  fImage->setDimensions(fineDims.data());
  fImage->setResolution(fineRes);
  fImage->setOrigin(fineOrig);
  DataContainer::Pointer refDC = DataContainer::New("ReferenceData");
  refDC->setGeometry(fImage);
  refDC->addAttributeMatrix(refAm->getName(), refAm);
  
  ImageGeom::Pointer cImage = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  cImage->setDimensions(coarseDims.data());
  cImage->setOrigin(coarseOrig);
  DataContainer::Pointer movDC = DataContainer::New("MovingData");
  movDC->setGeometry(cImage);
  movDC->addAttributeMatrix(movAm->getName(), movAm);
  
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(refDC);
  dca->addDataContainer(movDC);

  //create filter, execute, and check output
  QString filtName = "FuseVolumes";
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
    QString prefix("prefix_");

    std::vector< std::vector<double> > transform(3, std::vector<double>(4, 0));
    transform[0][0] = -1.0;
    transform[1][1] = -1.0;
    transform[2][2] = 1.0;
    transform[0][3] = cX;
    transform[1][3] = cY;
    DynamicTableData tableData;
    tableData.setTableData(transform);

    var.setValue(prefix);
    propWasSet = filter->setProperty("Prefix", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(1);//0: computed value, 1: manual entry
    propWasSet = filter->setProperty("TransformationType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(tableData);
    propWasSet = filter->setProperty("ManualTransformation", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(refDC->getName(), refAm->getName(), "");
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceVolume", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(movDC->getName(), movAm->getName(), "");
    var.setValue(path);
    propWasSet = filter->setProperty("MovingVolume", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter and check output
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    //get computed transformations and compare to applied transformations
    IDataArray::Pointer iFeatureIds = refDC->getAttributeMatrix(refAm->getName())->getAttributeArray(prefix + pCoarseIds->getName());
    DataArray<int32_t>* pFeatureIds = DataArray<int32_t>::SafePointerDownCast(iFeatureIds.get());
    DREAM3D_REQUIRE_VALID_POINTER(pFeatureIds)
    int32_t* regIds = pFeatureIds->getPointer(0);

    for(size_t i = 0; i < pFeatureIds->getNumberOfTuples(); i++) {
      DREAM3D_REQUIRE_EQUAL(fineIds[i], regIds[i])
    }
  }
  else
  {
    QString ss = QObject::tr("RegisterPointSetsTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
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
  QCoreApplication::setApplicationName("FuseVolumesTestTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( FuseVolumesTestTest() )

  PRINT_TEST_SUMMARY();
  return err;
}
