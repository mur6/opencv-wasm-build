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

# Utility rule file for gen_opencv_js_source.

# Include the progress variables for this target.
include modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/progress.make

modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source: modules/js_bindings_generator/gen/bindings.cpp
modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source: CMakeFiles/dephelper/gen_opencv_js_source


modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/js/src/core_bindings.cpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/js/generator/embindgen.py
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/js/generator/templates.py
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/python/src2/hdr_parser.py
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/affine.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/async.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/base.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/bindings_utils.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/bufferpool.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/check.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/core.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/cvstd.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/cvstd.inl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/cvstd_wrapper.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/detail/async_promise.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/detail/exception_ptr.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/directx.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/dualquaternion.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/dualquaternion.inl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/eigen.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/fast_math.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/mat.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/mat.inl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/matx.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/neon_utils.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/ocl_genbase.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/operations.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/optim.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/ovx.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/parallel/backend/parallel_for.openmp.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/parallel/backend/parallel_for.tbb.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/parallel/parallel_backend.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/persistence.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/private.cuda.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/quaternion.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/quaternion.inl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/saturate.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/simd_intrinsics.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/softfloat.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/sse_utils.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/traits.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/types.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utility.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/allocator_stats.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/allocator_stats.impl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/buffer_area.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/configuration.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/filesystem.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/filesystem.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/lock.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/logger.defines.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/logger.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/logtag.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/plugin_loader.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/utils/tls.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/va_intel.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/version.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/core/include/opencv2/core/vsx_utils.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc/bindings.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc/detail/gcgraph.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc/hal/hal.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc/imgproc.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/imgproc/include/opencv2/imgproc/segmentation.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/photo/include/opencv2/photo.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/photo/include/opencv2/photo/cuda.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/photo/include/opencv2/photo/photo.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/all_layers.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/dict.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/dnn.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/dnn.inl.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/layer.details.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/layer.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/layer_reg.private.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/shape_utils.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/utils/inference_engine.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/dnn/include/opencv2/dnn/version.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/features2d/include/opencv2/features2d.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/features2d/include/opencv2/features2d/features2d.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/calib3d/include/opencv2/calib3d.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/calib3d/include/opencv2/calib3d/calib3d.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/objdetect/include/opencv2/objdetect.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/objdetect/include/opencv2/objdetect/objdetect.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/video/include/opencv2/video.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/video/include/opencv2/video/background_segm.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/video/include/opencv2/video/detail/tracking.detail.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/video/include/opencv2/video/tracking.hpp
modules/js_bindings_generator/gen/bindings.cpp: /src/opencv-4.5.2/modules/video/include/opencv2/video/video.hpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/src/build_wasm_no_bulk_memory/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generate source files for JavaScript bindings"
	cd /src/build_wasm_no_bulk_memory/modules/js_bindings_generator/gen && /usr/bin/python3 /src/opencv-4.5.2/modules/js/generator/embindgen.py /src/opencv-4.5.2/modules/js/generator/../../python/src2/hdr_parser.py /src/build_wasm_no_bulk_memory/modules/js_bindings_generator/gen/bindings.cpp /src/build_wasm_no_bulk_memory/modules/js_bindings_generator/headers.txt /src/opencv-4.5.2/modules/js/generator/../src/core_bindings.cpp /src/opencv-4.5.2/platforms/js/opencv_js.config.py
	cd /src/build_wasm_no_bulk_memory/modules/js_bindings_generator/gen && /usr/bin/cmake -E touch /src/build_wasm_no_bulk_memory/CMakeFiles/dephelper/gen_opencv_js_source

CMakeFiles/dephelper/gen_opencv_js_source: modules/js_bindings_generator/gen/bindings.cpp
	@$(CMAKE_COMMAND) -E touch_nocreate CMakeFiles/dephelper/gen_opencv_js_source

gen_opencv_js_source: modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source
gen_opencv_js_source: modules/js_bindings_generator/gen/bindings.cpp
gen_opencv_js_source: CMakeFiles/dephelper/gen_opencv_js_source
gen_opencv_js_source: modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/build.make

.PHONY : gen_opencv_js_source

# Rule to build all files generated by this target.
modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/build: gen_opencv_js_source

.PHONY : modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/build

modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/clean:
	cd /src/build_wasm_no_bulk_memory/modules/js_bindings_generator && $(CMAKE_COMMAND) -P CMakeFiles/gen_opencv_js_source.dir/cmake_clean.cmake
.PHONY : modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/clean

modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/depend:
	cd /src/build_wasm_no_bulk_memory && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /src/opencv-4.5.2 /src/opencv-4.5.2/modules/js/generator /src/build_wasm_no_bulk_memory /src/build_wasm_no_bulk_memory/modules/js_bindings_generator /src/build_wasm_no_bulk_memory/modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/js_bindings_generator/CMakeFiles/gen_opencv_js_source.dir/depend

