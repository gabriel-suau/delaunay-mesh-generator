cmake_minimum_required(VERSION 3.10)
project(DMG)
enable_language(C)

set(DMG_VERSION_MAJOR "1")
set(DMG_VERSION_MINOR "0")
set(DMG_VERSION_PATCH "0")
set(DMG_RELEASE_DATE "Jan. 02, 2022")
set(DMG_VERSION "${DMG_VERSION_MAJOR}.${DMG_VERSION_MINOR}.${DMG_VERSION_PATCH}")

option(DMG_DOC "Build the doxygen documentation" OFF)

# set the main cmake compilation variables
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(DMG_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# include files
include_directories(${DMG_SOURCE_DIR})

# glob source files into one variable
file(GLOB DMG_SOURCE_FILES ${DMG_SOURCE_DIR}/*.c)

# include the math library
find_library(M_LIB m)
set(LIBRARIES ${M_LIB} ${LIBRARIES})
link_libraries(${LIBRARIES})

# add the executable
add_executable(dmg ${DMG_SOURCE_FILES})

# documentation
if (DMG_DOC)
  add_subdirectory(doc)
endif()

## TESTS
include(CTest)
if (BUILD_TESTING)
  set(DMG_MESHES_DIR ${PROJECT_SOURCE_DIR}/meshes)
  add_test(square ${EXECUTABLE_OUTPUT_PATH}/dmg ${DMG_MESHES_DIR}/input/square1D.mesh ${DMG_MESHES_DIR}/output/square.mesh)
  add_test(holy_square ${EXECUTABLE_OUTPUT_PATH}/dmg ${DMG_MESHES_DIR}/input/holy_square1D.mesh ${DMG_MESHES_DIR}/output/holy_square.mesh)
  add_test(kettle ${EXECUTABLE_OUTPUT_PATH}/dmg ${DMG_MESHES_DIR}/input/kettle1D.mesh ${DMG_MESHES_DIR}/output/kettle.mesh)
  add_test(not_convex ${EXECUTABLE_OUTPUT_PATH}/dmg ${DMG_MESHES_DIR}/input/not_convex1D.mesh ${DMG_MESHES_DIR}/output/not_convex.mesh)
endif()
