#!/bin/bash

case "$1" in
    --with-bulk-memory)
        mkdir -p build_wasm
        EXTRA_OPTIONS=""
        ;;
    --no-bulk-memory)
        mkdir -p build_wasm_no_bulk_memory
        EXTRA_OPTIONS='--build_flags="-mno-bulk-memory"'
        ;;
    *)
        echo "実行するには1個の引数 --with-bulk-memory または --no-bulk-memory が必要です。" 1>&2
        exit 1
esac

docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk:2.0.10 \
  emcmake python3 ./opencv-3.4.14/platforms/js/build_js.py build_wasm --build_wasm $EXTRA_OPTIONS
