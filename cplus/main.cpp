#include <emscripten.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "include/myjpeglib.h"

using namespace std;
Image *resize(Image &image, int max_size = 512)
{
    float ratio = min((float)min((float)max_size / image.width, (float)max_size / image.height), 1.0f);
    int new_width = image.width * ratio;
    int new_height = image.height * ratio;
    // Get a new buffer to interpolate into
    int width = image.width;
    int height = image.height;
    unsigned char *data = image.data;
    unsigned char *new_data = new unsigned char[new_width * new_height * 3];

    double scaleWidth = (double)new_width / (double)width;
    double scaleHeight = (double)new_height / (double)height;

    for (int cy = 0; cy < new_height; cy++)
    {
        for (int cx = 0; cx < new_width; cx++)
        {
            int pixel = (cy * (new_width * 3)) + (cx * 3);
            int nearestMatch = (((int)(cy / scaleHeight) * (width * 3)) + ((int)(cx / scaleWidth) * 3));
            new_data[pixel] = data[nearestMatch];
            new_data[pixel + 1] = data[nearestMatch + 1];
            new_data[pixel + 2] = data[nearestMatch + 2];
        }
    }
    Image *temp = new Image();
    temp->width = new_width;
    temp->height = new_height;
    temp->data = new_data;
    return temp;
}
extern "C"
{
    BYTE *EMSCRIPTEN_KEEPALIVE read_jpeg(uint8_t *buffer, size_t nSize)
    {
        Image *img_original = readJpeg(buffer, nSize);
        // BYTE data[img_original->compressedSize + 8];
        int size = img_original->height * img_original->width * 3;
        BYTE *data = new BYTE(size + 8);
        BYTE *height = intToBytes(img_original->height);
        memcpy(data, height, 4);
        BYTE *width = intToBytes(img_original->width);
        memcpy(data + 4, width, 4);
        memcpy(data + 8, img_original->data, size);
        return data;
    }
    BYTE *EMSCRIPTEN_KEEPALIVE readPng(uint8_t *buffer, size_t nSize)
    {
        Image *img_original = read_png(buffer, nSize);
        int size = img_original->height * img_original->width * 4;
        BYTE *data = new BYTE(size + 8);
        BYTE *height = intToBytes(img_original->height);
        memcpy(data, height, 4);
        BYTE *width = intToBytes(img_original->width);
        memcpy(data + 4, width, 4);
        memcpy(data + 8, img_original->data, size);
        return data;
    }
    BYTE *EMSCRIPTEN_KEEPALIVE resize_image(int width, int height, int compressedSize, BYTE *buffer)
    {
        Image *img_original = new Image(width, height, compressedSize, buffer);
        Image *img_resized = resize(*img_original);
        BYTE *data = writeJpeg(img_resized->data, img_resized->width, img_resized->height, 95);
        return data;
    }
    BYTE *EMSCRIPTEN_KEEPALIVE blend(int width, int height, BYTE *buffer, int mask_width, int mask_height,  BYTE* mask_buffer)
    {
        float ratioWidth = (float)width / (float)mask_width;
        float ratioHeight = (float)height / (float)mask_height;
        BYTE *data = new BYTE(height * width * 4);
        for(int i = 0; i < height; ++i) {
            for(int j = 0; j < width; ++j){
                int position = i * width * 3 + j * 3;
                int new_position = i * width * 4 + j * 4;
                int mask_position = round((float)i / ratioHeight) * mask_width + round((float)j / ratioWidth);
                data[new_position] = buffer[position];
                data[new_position + 1] = buffer[position + 1];
                data[new_position + 2] = buffer[position + 2];
                data[new_position + 3] = mask_buffer[mask_position * 4 + 3];
            }
        }
        return data;
    }
    int EMSCRIPTEN_KEEPALIVE print_tests()
    {
        EM_ASM(console.log('Hello from JS'););
        std::cout << "start test" << 'i' << 8 << std::endl;
        emscripten_log(EM_LOG_NO_PATHS, "Print a log message: int: %d, string: %s.", 42, "hello");
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_WARN, "Print a warning message");
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_ERROR, "This is an error!");

        // Log directly to Browser web inspector/console.
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE, "Info log to console: int: %d, string: %s", 42, "hello");
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_WARN, "Warning message to console.");
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE | EM_LOG_ERROR, "Error message to console! This should appear in red!");

        // Log to with full callstack information (both original C source and JS callstacks):
        emscripten_log(EM_LOG_C_STACK | EM_LOG_JS_STACK | EM_LOG_DEMANGLE, "A message with as full call stack information as possible:");

        // Log with just mangled JS callstacks:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_JS_STACK, "This is a message with a mangled JS callstack:");

        // Log only clean C callstack:
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_C_STACK | EM_LOG_DEMANGLE, "This message should have a clean C callstack:");

        // We can leave out the message to just print out the callstack:
        printf("The following line should show just the callstack without a message:\n");
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_ERROR | EM_LOG_C_STACK | EM_LOG_JS_STACK | EM_LOG_DEMANGLE);
        return 0;
    }
}