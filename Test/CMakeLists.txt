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

project(DataFusionTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support SIMPLib)

AddDREAM3DUnitTest(TESTNAME RegisterPointSetsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/RegisterPointSetsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
AddDREAM3DUnitTest(TESTNAME MatchFeatureIdsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/MatchFeatureIdsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
AddDREAM3DUnitTest(TESTNAME RegisterOrientationsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/RegisterOrientationsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs} OrientationLib)
AddDREAM3DUnitTest(TESTNAME FuseVolumesTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/FuseVolumesTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
