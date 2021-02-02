include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/bean)
#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/test/bin)

aux_source_directory(./ DIR_SRCS)

add_executable(tcpCLient ${DIR_SRCS})

set(LINK_LIB  pthread)

target_link_libraries(tcpCLient ${LINK_LIB})


