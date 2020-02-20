PROJECT(image)
cmake_minimum_required(VERSION 3.5.1)

SET(JPEG_INCLUDE_DIRS /Users/ma/buldlib/libjpeg-turbo/include)
FILE(GLOB JPEG_LIBS /Users/ma/buldlib/libjpeg-turbo/lib/*.bc)


SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_EXECUTABLE_SUFFIX ".js")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3 --llvm-lto 1 --bind -s ASSERTIONS=2 --memory-init-file 0 -s ALLOW_MEMORY_GROWTH=1")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_FILESYSTEM=1 -s NO_EXIT_RUNTIME=1")

include_directories(include ${JPEG_INCLUDE_DIRS})

add_library(myjpeg myjpeglib.cpp)
SET_TARGET_PROPERTIES(myjpeg PROPERTIES SUFFIX ".bc")
target_link_libraries(myjpeg ${JPEG_LIBS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} myjpeg)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                         COMMAND node ${CMAKE_SOURCE_DIR}/setWasmToJsFileAsBase64.js
                         ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
                         ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm ${PROJECT_NAME}
                         COMMENT "combine wasm and js file to single js file and store in wasmjs directory")