#!/bin/bash

docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk:2.0.10 \
  emcmake python3 ./opencv-3.4.14/platforms/js/build_js.py build_wasm_no_bulk_memory --build_wasm --build_flags="-mno-bulk-memory"
