# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/92/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/92/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhihuasun/Documents/ECE438/mp2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/mp2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mp2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mp2.dir/flags.make

CMakeFiles/mp2.dir/src/receiver_main.c.o: CMakeFiles/mp2.dir/flags.make
CMakeFiles/mp2.dir/src/receiver_main.c.o: ../src/receiver_main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/mp2.dir/src/receiver_main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/mp2.dir/src/receiver_main.c.o   -c /home/zhihuasun/Documents/ECE438/mp2/src/receiver_main.c

CMakeFiles/mp2.dir/src/receiver_main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mp2.dir/src/receiver_main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zhihuasun/Documents/ECE438/mp2/src/receiver_main.c > CMakeFiles/mp2.dir/src/receiver_main.c.i

CMakeFiles/mp2.dir/src/receiver_main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mp2.dir/src/receiver_main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zhihuasun/Documents/ECE438/mp2/src/receiver_main.c -o CMakeFiles/mp2.dir/src/receiver_main.c.s

# Object files for target mp2
mp2_OBJECTS = \
"CMakeFiles/mp2.dir/src/receiver_main.c.o"

# External object files for target mp2
mp2_EXTERNAL_OBJECTS =

mp2: CMakeFiles/mp2.dir/src/receiver_main.c.o
mp2: CMakeFiles/mp2.dir/build.make
mp2: CMakeFiles/mp2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable mp2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mp2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mp2.dir/build: mp2

.PHONY : CMakeFiles/mp2.dir/build

CMakeFiles/mp2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mp2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mp2.dir/clean

CMakeFiles/mp2.dir/depend:
	cd /home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhihuasun/Documents/ECE438/mp2 /home/zhihuasun/Documents/ECE438/mp2 /home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug /home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug /home/zhihuasun/Documents/ECE438/mp2/cmake-build-debug/CMakeFiles/mp2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mp2.dir/depend

