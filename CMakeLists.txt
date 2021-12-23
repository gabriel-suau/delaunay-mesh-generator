cmake_minimum_required(VERSION 3.10)

# set the project name
project(DMG VERSION 1.0)

# set the main cmake compilation variables
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(DMG_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# include files
include_directories(${DMG_SOURCE_DIR})

# glob source files into one variable
file(GLOB DMG_SOURCE_FILES ${DMG_SOURCE_DIR}/*.c)

# add compiler options
add_compile_options(-Wall)

# include the math library
find_library(M_LIB m)
set(LIBRARIES ${M_LIB} ${LIBRARIES})
link_libraries(${LIBRARIES})

# add the executable
add_executable(dmg ${DMG_SOURCE_FILES})
