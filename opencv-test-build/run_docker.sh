#!/bin/bash

if [ $# -ne 2 ]; then
  echo "実行するには2個の引数が必要です。" 1>&2
  exit 1
fi

if [ $1 = "3" ]; then
  OPENCV_BASE_PATH=$(realpath ../opencv-3.4.14)
elif [ $1 = "4" ]; then
  OPENCV_BASE_PATH=$(realpath ../opencv-4.5.2)
else
  echo "第１引数のバージョン 3 or 4を指定して下さい。" 1>&2
  exit 1
fi

OPENCV_BUILD=$2

docker run \
  --rm \
  -v $OPENCV_BASE_PATH:/src \
  -v $(pwd):/code \
  -w /code \
  -u $(id -u):$(id -g) \
  -e OPENCV_DIR="/src/$OPENCV_BUILD" \
  emscripten/emsdk:2.0.18 \
  bash docker-entrypoint.sh
