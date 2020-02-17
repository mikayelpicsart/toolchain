#!/bin/bash
mkdir -p ./cplus/build
rm -rf ./cplus/build/*
cd ./cplus/build/
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Users/ma/Projects/emsdk/fastcomp/emscripten/cmake/Modules/Platform/Emscripten.cmake
make