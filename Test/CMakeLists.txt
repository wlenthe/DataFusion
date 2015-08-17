#--////////////////////////////////////////////////////////////////////////////
#--
#--  PLEASE PROVIDE COPYRIGHT INFORMATION HERE.
#--
#--////////////////////////////////////////////////////////////////////////////

project(DataFusionTest)

include(${DREAM3DProj_SOURCE_DIR}/Test/DREAM3DTestFunctions.cmake)


include_directories(${${PROJECT_NAME}_SOURCE_DIR})
include_directories(${${PROJECT_NAME}_BINARY_DIR})


set(TEST_TEMP_DIR ${${PROJECT_NAME}_BINARY_DIR}/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})


configure_file(${${PROJECT_NAME}_SOURCE_DIR}/TestFileLocations.h.in
               ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}FileLocations.h @ONLY IMMEDIATE)

set(${PROJECT_NAME}_Link_Libs Qt5::Core H5Support DREAM3DLib)

AddDREAM3DUnitTest(TESTNAME RegisterPointSetsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/RegisterPointSetsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
AddDREAM3DUnitTest(TESTNAME MatchFeatureIdsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/MatchFeatureIdsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
AddDREAM3DUnitTest(TESTNAME RegisterOrientationsTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/RegisterOrientationsTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
AddDREAM3DUnitTest(TESTNAME FuseVolumesTest SOURCES ${${PROJECT_NAME}_SOURCE_DIR}/FuseVolumesTest.cpp LINK_LIBRARIES ${${PROJECT_NAME}_Link_Libs})
