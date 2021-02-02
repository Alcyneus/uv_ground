
add_subdirectory(./utils/catch)
add_subdirectory(./utils/tcpClient)

include_directories(${PROJECT_SOURCE_DIR}/src/configs)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)

include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/socket)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/bean)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/thread)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/can)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/gpio)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/bram)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/iio)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/iio/baseband)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/iio/baseband/basebandInit)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/common/iio/sync)

include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/tcpServer)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/simulate)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/simulate/commands)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/simulate/commands/downlink)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/simulate/commands/indirect)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/simulate/commands/polling)

include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/unitTest)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/unitTest/captureTest)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/unitTest/errorRateTest)
include_directories(${PROJECT_SOURCE_DIR}/src/headers/src/service/autoTest/unitTest/errorRateTest/bitSync)

include_directories(${PROJECT_SOURCE_DIR}/test/utils/catch)
include_directories(${PROJECT_SOURCE_DIR}/test/utils/log4cplus)
include_directories(${PROJECT_SOURCE_DIR}/test/utils/tcpClient)

include_directories(${PROJECT_SOURCE_DIR}/test/src/common/socket)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/bean)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/thread)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/can)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/gpio)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/bram)

include_directories(${PROJECT_SOURCE_DIR}/test/src/common/iio)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/iio/baseband)
include_directories(${PROJECT_SOURCE_DIR}/test/src/common/iio/sync)


include_directories(${PROJECT_SOURCE_DIR}/test/src/service/tcpServer)
include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest)
include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest/simulate)
include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest/simulate/commands)

include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest/unitTest/captureTest)
include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest/unitTest/errorRateTest)
include_directories(${PROJECT_SOURCE_DIR}/test/src/service/autoTest/unitTest/errorRateTest/bitSync)


#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/test/bin)

aux_source_directory(./ DIR_SRCS)

add_executable(test ${DIR_SRCS})

set(LINK_LIB pthread basebandInit iio)

target_link_libraries(test ${LINK_LIB})


