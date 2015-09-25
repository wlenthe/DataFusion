# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                             #
# Copyright (c) 2015 William Lenthe                                           #
#                                                                             #
# This program is free software: you can redistribute it and/or modify        #
# it under the terms of the GNU Lesser General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or           #
# (at your option) any later version.                                         #
#                                                                             #
# This program is distributed in the hope that it will be useful,             #
# but WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               #
# GNU Lesser General Public License for more details.                         #
#                                                                             #
# You should have received a copy of the GNU Lesser General Public License    #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.       #
#                                                                             #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

#project(DataFusionTest)

set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

include_directories(${DataFusionTest_SOURCE_DIR})

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}Test_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/Test/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/Test/${PROJECT_NAME}TestFileLocations.h @ONLY IMMEDIATE)

set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME RegisterPointSetsTest 
				  SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/RegisterPointSetsTest.cpp
				  FOLDER "${PLUGIN_NAME}Plugin/Test" 
				  LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME MatchFeatureIdsTest 
			      SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/MatchFeatureIdsTest.cpp 
			      FOLDER "${PLUGIN_NAME}Plugin/Test"
			      LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME RegisterOrientationsTest 
			      SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/RegisterOrientationsTest.cpp 
			      FOLDER "${PLUGIN_NAME}Plugin/Test"
			      LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs} OrientationLib)

AddDREAM3DUnitTest(TESTNAME FuseVolumesTest 
			      SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/FuseVolumesTest.cpp 
			      FOLDER "${PLUGIN_NAME}Plugin/Test"
			      LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})

AddDREAM3DUnitTest(TESTNAME RenumberFeaturesTest 
			      SOURCES ${${PLUGIN_NAME}Test_SOURCE_DIR}/RenumberFeaturesTest.cpp 
			      FOLDER "${PLUGIN_NAME}Plugin/Test"
			      LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
