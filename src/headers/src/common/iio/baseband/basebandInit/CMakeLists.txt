aux_source_directory(. DIR_LIB_SRCS)


#设置库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#生成库文件
add_library(basebandInit STATIC ${DIR_LIB_SRCS})
