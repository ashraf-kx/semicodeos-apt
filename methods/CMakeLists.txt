aux_source_directory(. SRC_LIST_METHODS)
add_library(methods ${SRC_LIST_METHODS})
include_directories("${PROJECT_SOURCE_DIR}/config.h")
include_directories("aptmethods.h")
