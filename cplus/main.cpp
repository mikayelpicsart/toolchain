#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <emscripten.h>
#include <iostream>

using namespace std;
using namespace cv;

extern "C"
{
    int EMSCRIPTEN_KEEPALIVE resize_image(uint8_t* buffer, size_t nSize )
    {
        Mat raw_data = cv::Mat(1, nSize, CV_8UC1, buffer);
        Mat test = imdecode(raw_data, cv::IMREAD_UNCHANGED);
        cout << test << endl;
        return 1;
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