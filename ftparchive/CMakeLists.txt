aux_source_directory(. SRC_LIST_FTPARCHIVE)
add_library(ftparchive ${SRC_LIST_FTPARCHIVE})
include_directories("${PROJECT_SOURCE_DIR}/config.h")
