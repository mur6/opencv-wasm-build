# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /src/opencv-4.5.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /src/build_wasm_no_bulk_memory

# Utility rule file for opencv.js.

# Include the progress variables for this target.
include modules/js/CMakeFiles/opencv.js.dir/progress.make

modules/js/CMakeFiles/opencv.js: bin/opencv.js
modules/js/CMakeFiles/opencv.js: bin/opencv_js.js


bin/opencv.js: bin/opencv_js.js
bin/opencv.js: /src/opencv-4.5.2/modules/js/src/make_umd.py
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/src/build_wasm_no_bulk_memory/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ../../bin/opencv.js"
	cd /src/build_wasm_no_bulk_memory/modules/js && /usr/bin/python3 /src/opencv-4.5.2/modules/js/src/make_umd.py /src/build_wasm_no_bulk_memory/bin/opencv_js.js /src/build_wasm_no_bulk_memory/bin/opencv.js

opencv.js: modules/js/CMakeFiles/opencv.js
opencv.js: bin/opencv.js
opencv.js: modules/js/CMakeFiles/opencv.js.dir/build.make

.PHONY : opencv.js

# Rule to build all files generated by this target.
modules/js/CMakeFiles/opencv.js.dir/build: opencv.js

.PHONY : modules/js/CMakeFiles/opencv.js.dir/build

modules/js/CMakeFiles/opencv.js.dir/clean:
	cd /src/build_wasm_no_bulk_memory/modules/js && $(CMAKE_COMMAND) -P CMakeFiles/opencv.js.dir/cmake_clean.cmake
.PHONY : modules/js/CMakeFiles/opencv.js.dir/clean

modules/js/CMakeFiles/opencv.js.dir/depend:
	cd /src/build_wasm_no_bulk_memory && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /src/opencv-4.5.2 /src/opencv-4.5.2/modules/js /src/build_wasm_no_bulk_memory /src/build_wasm_no_bulk_memory/modules/js /src/build_wasm_no_bulk_memory/modules/js/CMakeFiles/opencv.js.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/js/CMakeFiles/opencv.js.dir/depend

