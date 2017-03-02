cmake_minimum_required(VERSION 2.8)
# The version number.
set (SC_APT_VERSION_MAJOR 1)
set (SC_APT_VERSION_MINOR 0)
set(CMAKE_CXX_STANDARD 14)
# the configured options and settings for Tutorial
#define SC_APT_VERSION_MAJOR @SC_APT_VERSION_MAJOR@
#define SC_APT_VERSION_MINOR @SC_APT_VERSION_MINOR@

project(SC_APT)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories("${PROJECT_SOURCE_DIR}/config.h")
include_directories("${PROJECT_SOURCE_DIR}/apti18n.h")
# We add include_directories so that the headers files can be found for the functions prototypes.
include_directories ("${PROJECT_SOURCE_DIR}/apt-inst")
include_directories ("${PROJECT_SOURCE_DIR}/apt-private")
include_directories ("${PROJECT_SOURCE_DIR}/ftparchive")
include_directories ("${PROJECT_SOURCE_DIR}/methods")


add_subdirectory (apt-inst)
add_subdirectory (apt-private)
add_subdirectory (ftparchive)
add_subdirectory (methods)
#subdirs(apt-inst apt-private ftparchive methods)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
 # "${PROJECT_SOURCE_DIR}/config.h.in"
 # "${PROJECT_SOURCE_DIR}/config.h"
 # "${PROJECT_SOURCE_DIR}/apti18n.h.in"
 # "${PROJECT_SOURCE_DIR}/apti18n.h"
#  )
# add the binary tree to the search path for include files
# so that we will find config.h


link_directories(/usr/lib/x86_64-linux-gnu)


#The last change is to add the new library to the executable
target_link_libraries(${PROJECT_NAME} apt-pkg apt-inst apt-private ftparchive methods json curl)
