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

#include "SIMPLib/Math/QuaternionMath.hpp"
#include "OrientationLib/OrientationMath/OrientationConverter.hpp"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"

#include "DataFusionTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  //no input files
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TestFilterAvailability()
{
	// Now instantiate the RegisterOrientationsTest Filter from the FilterManager
	QString filtName = "RegisterOrientations";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The RegisterOrientationsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegisterOrientationsTest()
{
  //create grid of points in cubochoric
  size_t n = 3;//side length
  size_t numPoints = n * n * n;
  QVector<size_t> tDims(1, numPoints);
  QVector<size_t> cDims(1, 3);

  DataArray<float>::Pointer pOrientationGrid = DataArray<float>::CreateArray(tDims, cDims, "OrientationGrid");
  float* refCubo = pOrientationGrid->getPointer(0);
  size_t index = 0;
  for(size_t i = 0; i < n; i++) {
    for(size_t j = 0; j < n; j++) {
      for(size_t k = 0; k < n; k++) {
        refCubo[3 * index + 0] = static_cast<float>(i) / static_cast<float>(n - 1);
        refCubo[3 * index + 1] = static_cast<float>(j) / static_cast<float>(n - 1);
        refCubo[3 * index + 2] = static_cast<float>(k) / static_cast<float>(n - 1);
        index++;
      } 
    }
  }

  //convert orientation to quaternions
  CubochoricConverter<float>::Pointer converter = CubochoricConverter<float>::New();
  converter->setInputData(pOrientationGrid);
  converter->toQuaternion();
  DataArray<float>::Pointer pReferenceOrientations = converter->getOutputData();
  DREAM3D_REQUIRE_VALID_POINTER(pReferenceOrientations)
  pReferenceOrientations->setName("ReferenceQuats");

  //rotate orientations 135 deg @ [2, -1, 3]
  IDataArray::Pointer iRotatedOrientations = pReferenceOrientations->deepCopy();
  iRotatedOrientations->setName("RotatedQuats");
  QuatF* referenceOrientations = reinterpret_cast<QuatF*>(pReferenceOrientations->getPointer(0));
  QuatF* rotatedOrientations = reinterpret_cast<QuatF*>(DataArray<float>::SafePointerDownCast(iRotatedOrientations.get())->getPointer(0));

  double c = sqrt( (4.0 - sqrt(8.0)) / 8.0);//cos(67.5 deg)
  double s = sqrt( (4.0 + sqrt(8.0)) / 112.0);//sin(67.5 deg) / mag([2, -1, 3])
  QuatF rotation = QuaternionMathF::New(2.0 * s, -1.0 * s, 3.0 * s, c);

  for(size_t i = 0; i < numPoints; i++) {
    QuaternionMathF::Multiply(referenceOrientations[i], rotation, rotatedOrientations[i]);
  }

  //move reference crystal structures to FZ to introduce symmetry operators / make comparison easier
  unsigned int xtalStructure = Ebsd::CrystalStructure::Cubic_High;
  QVector<SpaceGroupOps::Pointer> ops = SpaceGroupOps::getOrientationOpsQVector();
  for(size_t i = 0; i < numPoints; i++) {
    ops[xtalStructure]->getFZQuat(referenceOrientations[i]);
  }    

  //create phases
  cDims[0] = 1;
  DataArray<int32_t>::Pointer pPhases = DataArray<int32_t>::CreateArray(tDims, cDims, DREAM3D::FeatureData::FeaturePhases);
  pPhases->initializeWithValue(1);

  //create feature attribute matrix and fill
  AttributeMatrix::Pointer featAM = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellFeatureAttributeMatrixName, DREAM3D::AttributeMatrixType::CellFeature);
  featAM->addAttributeArray(pReferenceOrientations->getName(), pReferenceOrientations);
  featAM->addAttributeArray(iRotatedOrientations->getName(), iRotatedOrientations);
  featAM->addAttributeArray(pPhases->getName(), pPhases);

  //create crystal structures
  tDims[0] = 2;
  DataArray<unsigned int>::Pointer pXtal = DataArray<unsigned int>::CreateArray(tDims, cDims, DREAM3D::EnsembleData::CrystalStructures);
  pXtal->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  pXtal->setValue(1, xtalStructure);

  //create ensemble attribute matrix and fill
  AttributeMatrix::Pointer ensbAM = AttributeMatrix::New(tDims, DREAM3D::Defaults::CellEnsembleAttributeMatrixName, DREAM3D::AttributeMatrixType::CellEnsemble);
  ensbAM->addAttributeArray(pXtal->getName(), pXtal);

  //create data container and fill
  DataContainer::Pointer dc = DataContainer::New("dc");
  dc->setGeometry(ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry));
  dc->addAttributeMatrix(featAM->getName(), featAM);
  dc->addAttributeMatrix(ensbAM->getName(), ensbAM);

  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(dc);

  //create filter, execute, and check results
  QString filtName = "RegisterOrientations";
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

    var.setValue(false);
    propWasSet = filter->setProperty("UseGoodFeatures", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(1.0f);
    propWasSet = filter->setProperty("MinMiso", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), featAM->getName(), pReferenceOrientations->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceAvgQuatsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), featAM->getName(), iRotatedOrientations->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("MovingAvgQuatsArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), featAM->getName(), pPhases->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferencePhasesArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    
    var.setValue(path);
    propWasSet = filter->setProperty("MovingPhasesArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), ensbAM->getName(), pXtal->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceCrystalStructuresArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(path);
    propWasSet = filter->setProperty("MovingCrystalStructuresArrayPath", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter and get registered orientations
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    IDataArray::Pointer iRegisteredOrientations = dc->getAttributeMatrix(featAM->getName())->getAttributeArray(iRotatedOrientations->getName());
    DataArray<float>* pRegisteredOrientations = DataArray<float>::SafePointerDownCast(iRegisteredOrientations.get());
    QuatF* registeredOrientations = reinterpret_cast<QuatF*>(pRegisteredOrientations->getPointer(0));
    DREAM3D_REQUIRE_VALID_POINTER(registeredOrientations)

    //move all registered quats into FZ to more easily compare (alternatively could use getMisoQuat)
    for(size_t i = 0; i < numPoints; i++) {
      ops[xtalStructure]->getFZQuat(registeredOrientations[i]);
    }

    //compare registered quats
    for(size_t i = 0; i < numPoints; i++) {
      DREAM3D_REQUIRE_EQUAL(0, 1)
      // DREAM3D_COMPARE_FLOATS(&referenceOrientations[i].x, &registeredOrientations[i].x, 10)
      // DREAM3D_COMPARE_FLOATS(&referenceOrientations[i].y, &registeredOrientations[i].y, 10)
      // DREAM3D_COMPARE_FLOATS(&referenceOrientations[i].z, &registeredOrientations[i].z, 10)
      // DREAM3D_COMPARE_FLOATS(&referenceOrientations[i].w, &registeredOrientations[i].w, 10)
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
  QCoreApplication::setApplicationName("RegisterOrientationsTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( RegisterOrientationsTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
