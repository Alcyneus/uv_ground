# Add the source file path
add_subdirectory(./headers/src/common/iio/baseband/basebandInit)

# Add the header file path
include_directories(${PROJECT_SOURCE_DIR}/src/configs)
include_directories(${PROJECT_SOURCE_DIR}/src/headers)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/src/bin)

# Generate an executable file
aux_source_directory(./ DIR_SRCS)
add_executable(exe ${DIR_SRCS})

# Link libraries
set(LINK_LIB  pthread)
target_link_libraries(exe ${LINK_LIB})


