cmake_minimum_required (VERSION 3.10)

project (apollo2csv)

set (apollo2csv_version_major 0)
set (apollo2csv_version_minor 6)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

configure_file (
  "${PROJECT_SOURCE_DIR}/apollo2csvConfig.h.in"
  "${PROJECT_SOURCE_DIR}/apollo2csvConfig.h"
)

include_directories ("${PROJECT_BINARY_DIR}")
link_libraries ("-static")
add_executable (apollo2csv apollo2csv.cxx apolloConvertConfig.cxx)
target_link_libraries (apollo2csv)

set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
