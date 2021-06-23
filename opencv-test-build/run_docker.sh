#!/bin/bash

if [ $# -ne 1 ]; then
  echo "実行するには1個の引数が必要です。" 1>&2
  exit 1
fi

OPENCV_DIR=$1

docker run \
  --rm \
  -v $(realpath ../opencv-3.4.14):/src \
  -v $(pwd):/code \
  -w /code \
  -u $(id -u):$(id -g) \
  -e OPENCV_DIR="/src/$OPENCV_DIR" \
  emscripten/emsdk:2.0.18 \
  bash docker-entrypoint.sh
