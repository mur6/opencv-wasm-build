#!/bin/bash

docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk:2.0.18 \
  emcc main.cpp -o main.js
