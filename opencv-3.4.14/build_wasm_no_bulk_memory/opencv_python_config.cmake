
set(CMAKE_BUILD_TYPE "Release")

set(BUILD_SHARED_LIBS "OFF")

set(CMAKE_C_FLAGS "-s WASM=1 -s USE_PTHREADS=0 -mno-bulk-memory   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winconsistent-missing-override -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -Wno-deprecated-enum-enum-conversion -Wno-deprecated-anon-enum-enum-conversion -fdiagnostics-show-option -Qunused-arguments -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_C_FLAGS_DEBUG "-g  -O0 -DDEBUG -D_DEBUG")

set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2  -DNDEBUG")

set(CMAKE_CXX_FLAGS "-s WASM=1 -s USE_PTHREADS=0 -mno-bulk-memory   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winconsistent-missing-override -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -Wno-deprecated-enum-enum-conversion -Wno-deprecated-anon-enum-enum-conversion -fdiagnostics-show-option -Qunused-arguments -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS_DEBUG "-g  -O0 -DDEBUG -D_DEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2  -DNDEBUG")

set(CV_GCC "")

set(CV_CLANG "1")

set(ENABLE_NOISY_WARNINGS "OFF")

set(CMAKE_MODULE_LINKER_FLAGS "")

set(CMAKE_INSTALL_PREFIX "/src/build_wasm_no_bulk_memory/install")

set(OPENCV_PYTHON_INSTALL_PATH "")

set(OpenCV_SOURCE_DIR "/src/opencv-3.4.14")

set(OPENCV_FORCE_PYTHON_LIBS "")

set(OPENCV_PYTHON_SKIP_LINKER_EXCLUDE_LIBS "")

set(OPENCV_PYTHON_BINDINGS_DIR "/src/build_wasm_no_bulk_memory/modules/python_bindings_generator")

set(cv2_custom_hdr "/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_custom_headers.h")

set(cv2_generated_files "/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_enums.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_funcs.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_include.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_modules.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_modules_content.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_types.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_generated_types_content.h;/src/build_wasm_no_bulk_memory/modules/python_bindings_generator/pyopencv_signatures.json")
