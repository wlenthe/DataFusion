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

#include <Eigen/Dense>

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
	// Now instantiate the RegisterPointSetsTest Filter from the FilterManager
	QString filtName = "RegisterPointSets";
	FilterManager* fm = FilterManager::Instance();
	IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
	if (NULL == filterFactory.get())
	{
		std::stringstream ss;
		ss << "The RegisterPointSetsTest Requires the use of the " << filtName.toStdString() << " filter which is found in the DataFusion Plugin";
		DREAM3D_TEST_THROW_EXCEPTION(ss.str())
	}
	return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RegisterPointSetsTest()
{
  //create input points: (2n + 1)^3 grid of points with spacing 1, centered at (0, 0, 0)
  static const int n = 2;
  static const int numPoints = (2 * n + 1) * (2 * n + 1) * (2 * n + 1);
  QVector<size_t> tDims(1, numPoints);
  QVector<size_t> cDims(1, 3);
  DataArray<float>::Pointer pReferencePoints = DataArray<float>::CreateArray(tDims, cDims, "ReferenceCentroids");
  DataArray<float>::Pointer pRotatedPoints = DataArray<float>::CreateArray(tDims, cDims, "RotatedCentroids");
  DataArray<float>::Pointer pShearedPoints = DataArray<float>::CreateArray(tDims, cDims, "ShearedCentroids");
  float* refPoints = pReferencePoints->getPointer(0);
  float* rotPoints = pRotatedPoints->getPointer(0);
  float* shrPoints = pShearedPoints->getPointer(0);
  size_t index = 0;
  for(int i = -n; i <= n; i++) {
    for(int j = -n; j <= n; j++) {
      for(int k = -n; k <= n; k++) {
        refPoints[3 * index + 0] = i;
        refPoints[3 * index + 1] = j;
        refPoints[3 * index + 2] = k;
        index++;
      }
    }
  }

  //create 2 transformations
    //no shear: rotate 45 @ 001, rotate 60 @ 100, shift (10, -10, 0)
    float translation[3] = {10.0, -10.0, 0.0};
    float rotation[9] = {sqrt(0.5), -sqrt(0.125),  sqrt(0.375),
                         sqrt(0.5),  sqrt(0.125), -sqrt(0.375),
                              0.0f,   sqrt(0.75),        0.5f};

    //with shear: rotate 45 @ 001, scale (2, 4, 8), rotate 60 @ 100, shift (10, -10, 0)
    float shearing[9] = {sqrt(2.0), -sqrt(2.0),  sqrt(6.0),
                         sqrt(2.0),  sqrt(2.0), -sqrt(6.0),
                              0.0f, sqrt(48.0),      4.0f};

  //fill in transformed points
  for(size_t i = 0; i < numPoints; i++) {
    for(size_t j = 0; j < 3; j++) {
      rotPoints[3 * i + j] = refPoints[3 * i + 0] * rotation[3 * j + 0] + refPoints[3 * i + 1] * rotation[3 * j + 1] + refPoints[3 * i + 2] * rotation[3 * j + 2] + translation[j];
      shrPoints[3 * i + j] = refPoints[3 * i + 0] * shearing[3 * j + 0] + refPoints[3 * i + 1] * shearing[3 * j + 1] + refPoints[3 * i + 2] * shearing[3 * j + 2] + translation[j];
    }
  }

  //create + fill data container
  AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, DREAM3D::Defaults::FeatureAttributeMatrixName, DREAM3D::AttributeMatrixObjectType::Feature);
  am->addAttributeArray(pReferencePoints->getName(), pReferencePoints);
  am->addAttributeArray(pRotatedPoints->getName(), pRotatedPoints);
  am->addAttributeArray(pShearedPoints->getName(), pShearedPoints);
  DataContainer::Pointer dc = DataContainer::New(DREAM3D::Defaults::DataContainerName);
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
  // image->setDimensions(dims);
  dc->setGeometry(image);
  dc->addAttributeMatrix(am->getName(), am);
  DataContainerArray::Pointer dca = DataContainerArray::New();
  dca->addDataContainer(dc);

  //output names
  QString rotationAM("RotationTransform"), shearingAM("ShearTransform"), transformName("Transformation");

  //create register point sets filter and execute
  QString filtName = "RegisterPointSets";
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
    propWasSet = filter->setProperty("UseGoodPoints", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)
    
    var.setValue(false);
    propWasSet = filter->setProperty("UseWeights", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = filter->setProperty("AllowTranslation", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = filter->setProperty("AllowRotation", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(true);
    propWasSet = filter->setProperty("AllowScaling", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(1);//0: isotropic, 1: anisotropic
    propWasSet = filter->setProperty("ScalingType", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(false);//no shearing on first run
    propWasSet = filter->setProperty("AllowShearing", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), am->getName(), pRotatedPoints->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceCentroidsArrayPath", var);//computes transform from moving->refernce so we need to swap
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), am->getName(), pReferencePoints->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("MovingCentroidsArrayPath", var);//computes transform from moving->refernce so we need to swap
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(rotationAM);
    propWasSet = filter->setProperty("AttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(transformName);
    propWasSet = filter->setProperty("TransformName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    //execute filter first time and check condition
    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    //reconfigure and execute filter
    var.setValue(true);//shearing on second run
    propWasSet = filter->setProperty("AllowShearing", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    path.update(dc->getName(), am->getName(), pShearedPoints->getName());
    var.setValue(path);
    propWasSet = filter->setProperty("ReferenceCentroidsArrayPath", var);//computes transform from moving->refernce so we need to swap
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    var.setValue(shearingAM);
    propWasSet = filter->setProperty("AttributeMatrixName", var);
    DREAM3D_REQUIRE_EQUAL(propWasSet, true)

    filter->execute();
    DREAM3D_REQUIRED(filter->getErrorCondition(), >= , 0);

    //get computed transformations and compare to applied transformations
    IDataArray::Pointer iRotation = dc->getAttributeMatrix(rotationAM)->getAttributeArray(transformName);
    IDataArray::Pointer iShearing = dc->getAttributeMatrix(shearingAM)->getAttributeArray(transformName);

    DataArray<float>* pRotation = DataArray<float>::SafePointerDownCast(iRotation.get());
    DataArray<float>* pShearing = DataArray<float>::SafePointerDownCast(iShearing.get());
    
    DREAM3D_REQUIRE_VALID_POINTER(pRotation)
    DREAM3D_REQUIRE_VALID_POINTER(pShearing)

    float* rotTransform = pRotation->getPointer(0);
    float* shrTransform = pShearing->getPointer(0);

    //compare row at a time
    for(size_t i = 0; i < 3; i++) {
      DREAM3D_REQUIRE_EQUAL(0, 1)
      // DREAM3D_COMPARE_FLOATS(&rotation[3 * i + 0], &rotTransform[4 * i + 0], 0xFFFFFFFF)
      // DREAM3D_COMPARE_FLOATS(&rotation[3 * i + 0], &rotTransform[4 * i + 0], 1000000000000)
      // DREAM3D_COMPARE_FLOATS(&rotation[3 * i + 1], &rotTransform[4 * i + 1], 100)
      // DREAM3D_COMPARE_FLOATS(rotation[3 * i + 2], rotTransform[4 * i + 2], 10)
      // DREAM3D_COMPARE_FLOATS(translation[i], rotTransform[4 * i + 3], 10)

      // DREAM3D_COMPARE_FLOATS(shearing[3 * i + 0], shrTransform[4 * i + 0], 10)
      // DREAM3D_COMPARE_FLOATS(shearing[3 * i + 1], shrTransform[4 * i + 1], 10)
      // DREAM3D_COMPARE_FLOATS(shearing[3 * i + 2], shrTransform[4 * i + 2], 10)
      // DREAM3D_COMPARE_FLOATS(translation[i], shrTransform[4 * i + 3], 10)
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
  QCoreApplication::setApplicationName("RegisterPointSetsTest");

  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( loadFilterPlugins() );
  DREAM3D_REGISTER_TEST( TestFilterAvailability() );

  DREAM3D_REGISTER_TEST( RegisterPointSetsTest() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
