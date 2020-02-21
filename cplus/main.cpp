#include <emscripten.h>
#include <iostream>
#include "include/myjpeglib.h"

using namespace std;
unsigned char * resize(int new_width, int new_height, char* data, int width, int height)
{
    // Get a new buffer to interpolate into
    unsigned char* new_data = new unsigned char[new_width * new_height * 3];

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
    return new_data;
}

extern "C"
{
    int EMSCRIPTEN_KEEPALIVE resize_image(uint8_t *buffer, size_t nSize)
    {
        Image *img = readJpeg(buffer, nSize);
        emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE, "%d, %d, %d", img->height, img->width, img->compressedSize);
        for (int i = 0; i < img->width; ++i)
        {
            emscripten_log(EM_LOG_NO_PATHS | EM_LOG_CONSOLE, "%d %d %d", img->data[i * 3], img->data[i * 3 + 1], img->data[i * 3 + 2]);
        }

        return 0;
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