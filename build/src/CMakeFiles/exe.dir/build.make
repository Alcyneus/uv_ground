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
include src/CMakeFiles/exe.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/exe.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/exe.dir/flags.make

src/CMakeFiles/exe.dir/Application.cpp.o: src/CMakeFiles/exe.dir/flags.make
src/CMakeFiles/exe.dir/Application.cpp.o: ../src/Application.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/exe.dir/Application.cpp.o"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++ --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/exe.dir/Application.cpp.o -c /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/Application.cpp

src/CMakeFiles/exe.dir/Application.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exe.dir/Application.cpp.i"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++ --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/Application.cpp > CMakeFiles/exe.dir/Application.cpp.i

src/CMakeFiles/exe.dir/Application.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exe.dir/Application.cpp.s"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src && /opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin/arm-linux-gnueabihf-g++ --sysroot=/opt/Xilinx/SDK/2018.3/gnu/aarch32/lin/gcc-arm-linux-gnueabi/sysroot $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src/Application.cpp -o CMakeFiles/exe.dir/Application.cpp.s

# Object files for target exe
exe_OBJECTS = \
"CMakeFiles/exe.dir/Application.cpp.o"

# External object files for target exe
exe_EXTERNAL_OBJECTS =

src/bin/exe: src/CMakeFiles/exe.dir/Application.cpp.o
src/bin/exe: src/CMakeFiles/exe.dir/build.make
src/bin/exe: src/CMakeFiles/exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/exe"
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/exe.dir/build: src/bin/exe

.PHONY : src/CMakeFiles/exe.dir/build

src/CMakeFiles/exe.dir/clean:
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src && $(CMAKE_COMMAND) -P CMakeFiles/exe.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/exe.dir/clean

src/CMakeFiles/exe.dir/depend:
	cd /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ls/Documents/UV_PROJECT/plutosdrTemplate1 /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/src /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src /home/ls/Documents/UV_PROJECT/plutosdrTemplate1/build/src/CMakeFiles/exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/exe.dir/depend
