cmake_minimum_required(VERSION 3.10)

find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  message(STATUS "Looking for doxygen - found")
  configure_file("Doxyfile.in" "Doxyfile" @ONLY)
  add_custom_command(OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/html
    COMMAND ${DOXYGEN_EXECUTABLE}
    ARGS    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    )
  add_custom_target(doc ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html
    )
else()
  message(FATAL_ERROR "Looking for doxygen - not found")
endif()
