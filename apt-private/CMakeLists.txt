aux_source_directory(. SRC_LIST_PRIVATE)
add_library(apt-private ${SRC_LIST_PRIVATE})
include_directories("${PROJECT_SOURCE_DIR}/config.h")
