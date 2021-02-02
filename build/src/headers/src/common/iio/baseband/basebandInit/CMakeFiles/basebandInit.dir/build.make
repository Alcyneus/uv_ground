# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/cmake-3.18.1-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /opt/cmake-3.18.1-Linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ls/Documents/UV_PROJECT/plutosdrTemplate1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build

# Include any dependencies generated for this target.
include src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/depend.make

# Include the progress variables for this target.
include src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/progress.make

# Include the compile flags for this target's objects.
include src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/flags.make

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.o: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/flags.make
src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.o: ../src/headers/src/common/iio/baseband/basebandInit/baseband_param_calc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.o"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/basebandInit.dir/baseband_param_calc.c.o -c /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/baseband_param_calc.c

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/basebandInit.dir/baseband_param_calc.c.i"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/baseband_param_calc.c > CMakeFiles/basebandInit.dir/baseband_param_calc.c.i

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/basebandInit.dir/baseband_param_calc.c.s"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/baseband_param_calc.c -o CMakeFiles/basebandInit.dir/baseband_param_calc.c.s

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.o: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/flags.make
src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.o: ../src/headers/src/common/iio/baseband/basebandInit/uv_config.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.o"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/basebandInit.dir/uv_config.c.o -c /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/uv_config.c

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/basebandInit.dir/uv_config.c.i"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/uv_config.c > CMakeFiles/basebandInit.dir/uv_config.c.i

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/basebandInit.dir/uv_config.c.s"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-gcc --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit/uv_config.c -o CMakeFiles/basebandInit.dir/uv_config.c.s

# Object files for target basebandInit
basebandInit_OBJECTS = \
"CMakeFiles/basebandInit.dir/baseband_param_calc.c.o" \
"CMakeFiles/basebandInit.dir/uv_config.c.o"

# External object files for target basebandInit
basebandInit_EXTERNAL_OBJECTS =

lib/libbasebandInit.a: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/baseband_param_calc.c.o
lib/libbasebandInit.a: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/uv_config.c.o
lib/libbasebandInit.a: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/build.make
lib/libbasebandInit.a: src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library ../../../../../../../lib/libbasebandInit.a"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && $(CMAKE_COMMAND) -P CMakeFiles/basebandInit.dir/cmake_clean_target.cmake
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/basebandInit.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/build: lib/libbasebandInit.a

.PHONY : src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/build

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/clean:
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit && $(CMAKE_COMMAND) -P CMakeFiles/basebandInit.dir/cmake_clean.cmake
.PHONY : src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/clean

src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/depend:
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ls/Documents/UV_PROJECT/plutosdrTemplate1 /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/headers/src/common/iio/baseband/basebandInit /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/headers/src/common/iio/baseband/basebandInit/CMakeFiles/basebandInit.dir/depend

