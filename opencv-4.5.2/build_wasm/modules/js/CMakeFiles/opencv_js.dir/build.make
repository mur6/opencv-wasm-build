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
CMAKE_BINARY_DIR = /src/build_wasm

# Include any dependencies generated for this target.
include modules/js/CMakeFiles/opencv_js.dir/depend.make

# Include the progress variables for this target.
include modules/js/CMakeFiles/opencv_js.dir/progress.make

# Include the compile flags for this target's objects.
include modules/js/CMakeFiles/opencv_js.dir/flags.make

modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o: modules/js/CMakeFiles/opencv_js.dir/flags.make
modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o: modules/js/CMakeFiles/opencv_js.dir/includes_CXX.rsp
modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o: modules/js_bindings_generator/gen/bindings.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/src/build_wasm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o"
	cd /src/build_wasm/modules/js && /emsdk/upstream/emscripten/em++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o -c /src/build_wasm/modules/js_bindings_generator/gen/bindings.cpp

modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.i"
	cd /src/build_wasm/modules/js && /emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /src/build_wasm/modules/js_bindings_generator/gen/bindings.cpp > CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.i

modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.s"
	cd /src/build_wasm/modules/js && /emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /src/build_wasm/modules/js_bindings_generator/gen/bindings.cpp -o CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.s

# Object files for target opencv_js
opencv_js_OBJECTS = \
"CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o"

# External object files for target opencv_js
opencv_js_EXTERNAL_OBJECTS =

bin/opencv_js.js: modules/js/CMakeFiles/opencv_js.dir/__/js_bindings_generator/gen/bindings.cpp.o
bin/opencv_js.js: modules/js/CMakeFiles/opencv_js.dir/build.make
bin/opencv_js.js: lib/libopencv_core.a
bin/opencv_js.js: lib/libopencv_flann.a
bin/opencv_js.js: lib/libopencv_imgproc.a
bin/opencv_js.js: lib/libopencv_photo.a
bin/opencv_js.js: lib/libopencv_dnn.a
bin/opencv_js.js: lib/libopencv_features2d.a
bin/opencv_js.js: lib/libopencv_calib3d.a
bin/opencv_js.js: lib/libopencv_objdetect.a
bin/opencv_js.js: lib/libopencv_video.a
bin/opencv_js.js: 3rdparty/lib/libquirc.a
bin/opencv_js.js: lib/libopencv_dnn.a
bin/opencv_js.js: 3rdparty/lib/liblibprotobuf.a
bin/opencv_js.js: lib/libopencv_calib3d.a
bin/opencv_js.js: lib/libopencv_features2d.a
bin/opencv_js.js: lib/libopencv_flann.a
bin/opencv_js.js: lib/libopencv_imgproc.a
bin/opencv_js.js: lib/libopencv_core.a
bin/opencv_js.js: 3rdparty/lib/libzlib.a
bin/opencv_js.js: modules/js/CMakeFiles/opencv_js.dir/linklibs.rsp
bin/opencv_js.js: modules/js/CMakeFiles/opencv_js.dir/objects1.rsp
bin/opencv_js.js: modules/js/CMakeFiles/opencv_js.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/src/build_wasm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/opencv_js.js"
	cd /src/build_wasm/modules/js && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_js.dir/link.txt --verbose=$(VERBOSE)
	cd /src/build_wasm/modules/js && /usr/bin/cmake -E copy /src/opencv-4.5.2/modules/js/src/loader.js /src/build_wasm/bin/loader.js

# Rule to build all files generated by this target.
modules/js/CMakeFiles/opencv_js.dir/build: bin/opencv_js.js

.PHONY : modules/js/CMakeFiles/opencv_js.dir/build

modules/js/CMakeFiles/opencv_js.dir/clean:
	cd /src/build_wasm/modules/js && $(CMAKE_COMMAND) -P CMakeFiles/opencv_js.dir/cmake_clean.cmake
.PHONY : modules/js/CMakeFiles/opencv_js.dir/clean

modules/js/CMakeFiles/opencv_js.dir/depend:
	cd /src/build_wasm && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /src/opencv-4.5.2 /src/opencv-4.5.2/modules/js /src/build_wasm /src/build_wasm/modules/js /src/build_wasm/modules/js/CMakeFiles/opencv_js.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/js/CMakeFiles/opencv_js.dir/depend

