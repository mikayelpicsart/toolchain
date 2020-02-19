PROJECT(image)
cmake_minimum_required(VERSION 3.5.1)
SET(OpenCV_INCLUDE_DIRS /Users/ma/project1/buildwithcodecs/install/include/opencv4)
FILE(GLOB OpenCV_LIBS /Users/ma/project1/buildwithcodecs/install/lib/*.bc)
include_directories(include ${OpenCV_INCLUDE_DIRS})
SET(CMAKE_EXECUTABLE_SUFFIX ".js")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3 --llvm-lto 1 --bind -s ASSERTIONS=1 -s NO_EXIT_RUNTIME=1 --memory-init-file 0 -s ALLOW_MEMORY_GROWTH=1 -s USE_LIBPNG=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0")
add_executable(${PROJECT_NAME} main.cpp ${OpenCV_LIBS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                         COMMAND node ${CMAKE_SOURCE_DIR}/setWasmToJsFileAsBase64.js
                         ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
                         ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm ${PROJECT_NAME}
                         COMMENT "combine wasm and js file to single js file and store in wasmjs directory")