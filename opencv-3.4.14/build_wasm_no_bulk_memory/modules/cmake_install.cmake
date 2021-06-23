# Install script for directory: /src/opencv-3.4.14/modules

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/src/build_wasm_no_bulk_memory/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/calib3d/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/core/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudaarithm/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudabgsegm/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudacodec/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudafeatures2d/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudafilters/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudaimgproc/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudalegacy/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudaobjdetect/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudaoptflow/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudastereo/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudawarping/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/cudev/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/dnn/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/features2d/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/flann/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/highgui/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/imgcodecs/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/imgproc/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/java/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/js/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/ml/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/objdetect/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/photo/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/python/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/shape/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/stitching/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/superres/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/ts/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/video/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/videoio/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/videostab/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/viz/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/.firstpass/world/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/core/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/imgproc/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/python_tests/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/video/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/dnn/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/features2d/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/calib3d/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/objdetect/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/java_bindings_generator/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/js_bindings_generator/cmake_install.cmake")
  include("/src/build_wasm_no_bulk_memory/modules/js/cmake_install.cmake")

endif()
