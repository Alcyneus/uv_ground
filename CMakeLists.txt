SET( CMAKE_CXX_FLAGS "-std=c++11 -O3")
cmake_minimum_required(VERSION 3.10)

#工程名称（）
project(PROJECT)

set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR arm)


#Whether to use cross-compilation
set(CROSSCOMPILING TRUE)
if(${CROSSCOMPILING})
    #指定交叉编译器路径
    set(TOOLSCHAIN_PATH "/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi")
    set(TOOLCHAIN_HOST "${TOOLSCHAIN_PATH}/bin/arm-linux-gnueabihf")

    #CMAKE_SYSROOT只有在3.0以上的版本才有效
    SET(CMAKE_SYSROOT "${TOOLSCHAIN_PATH}/sysroot")

    set(TOOLCHAIN_CC "${TOOLCHAIN_HOST}-gcc")
    set(TOOLCHAIN_CXX "${TOOLCHAIN_HOST}-g++")

    #告诉cmake是进行交叉编译
    set(CMAKE_CROSSCOMPILING TRUE)

    # Define name of the target system
    set(CMAKE_SYSTEM_NAME "Linux")
    # Define the compiler
    set(CMAKE_C_COMPILER ${TOOLCHAIN_CC})
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_CXX})

    #库和同头文件查找的路径。
    set(CMAKE_FIND_ROOT_PATH "${SYSROOT_PATH}" "${CMAKE_PREFIX_PATH}" "${TOOLSCHAIN_PATH}")

    # search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    # for libraries and headers in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)    
endif()


add_subdirectory(./src)
add_subdirectory(./test)

