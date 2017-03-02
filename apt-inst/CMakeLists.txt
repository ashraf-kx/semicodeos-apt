aux_source_directory(. SRC_LIST_INST)
add_library(apt-inst ${SRC_LIST_INST})
include_directories("${PROJECT_SOURCE_DIR}/config.h")
