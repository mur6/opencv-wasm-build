#!/bin/bash

mkdir -p build_wasm
cd build_wasm
emcmake cmake -DOpenCV_DIR=$OPENCV_DIR ..
emmake make
