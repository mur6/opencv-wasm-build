#!/bin/bash

case "$1" in
    --with-bulk-memory)
        BUILD_DIR="build_wasm"
        EXTRA_OPTIONS=""
        ;;
    --no-bulk-memory)
        BUILD_DIR="build_wasm_no_bulk_memory"
        EXTRA_OPTIONS='--build_flags="-mno-bulk-memory"'
        ;;
    *)
        echo "実行するには1個の引数 --with-bulk-memory または --no-bulk-memory が必要です。" 1>&2
        exit 1
esac

mkdir -p $BUILD_DIR
docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk:2.0.10 \
  emcmake python3 ./opencv-4.5.2/platforms/js/build_js.py $BUILD_DIR --build_wasm $EXTRA_OPTIONS
