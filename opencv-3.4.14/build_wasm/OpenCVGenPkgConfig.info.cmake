
set(BUILD_SHARED_LIBS "OFF")

set(CMAKE_BINARY_DIR "/src/build_wasm")

set(CMAKE_INSTALL_PREFIX "/src/build_wasm/install")

set(OpenCV_SOURCE_DIR "/src/opencv-3.4.14")

set(OPENCV_PC_FILE_NAME "opencv.pc")

set(OPENCV_VERSION_PLAIN "3.4.14")

set(OPENCV_LIB_INSTALL_PATH "lib")

set(OPENCV_INCLUDE_INSTALL_PATH "include")

set(OPENCV_3P_LIB_INSTALL_PATH "share/OpenCV/3rdparty/lib")

set(_modules "opencv_dnn;opencv_objdetect;opencv_calib3d;opencv_features2d;opencv_video;opencv_imgproc;opencv_core")

set(_extra "")

set(_3rdparty "zlib;libprotobuf;quirc")

set(TARGET_LOCATION_opencv_dnn "/src/build_wasm/lib/libopencv_dnn.a")

set(TARGET_LOCATION_opencv_objdetect "/src/build_wasm/lib/libopencv_objdetect.a")

set(TARGET_LOCATION_opencv_calib3d "/src/build_wasm/lib/libopencv_calib3d.a")

set(TARGET_LOCATION_opencv_features2d "/src/build_wasm/lib/libopencv_features2d.a")

set(TARGET_LOCATION_opencv_video "/src/build_wasm/lib/libopencv_video.a")

set(TARGET_LOCATION_opencv_imgproc "/src/build_wasm/lib/libopencv_imgproc.a")

set(TARGET_LOCATION_opencv_core "/src/build_wasm/lib/libopencv_core.a")

set(TARGET_LOCATION_zlib "/src/build_wasm/3rdparty/lib/libzlib.a")

set(TARGET_LOCATION_libprotobuf "/src/build_wasm/3rdparty/lib/liblibprotobuf.a")

set(TARGET_LOCATION_quirc "/src/build_wasm/3rdparty/lib/libquirc.a")
