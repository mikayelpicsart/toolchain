#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <emscripten.h>
#include <iostream>

using namespace std;
using namespace cv;

extern "C"
{
    // int EMSCRIPTEN_KEEPALIVE resize_image(char *const ptr, int len)
    // {
    //     cout << "start resize_image" << endl;
    //     // for (int i = 0; i < 10; i++)
    //     // {
    //     //     cout << (int)ptr[i] << endl;
    //     // }
    //     cout << "end resize_image" << endl;
    //     return 0;
    // }
    int EMSCRIPTEN_KEEPALIVE test()
    {
        std::cout << "start test" << std::endl;
        return 0;
    }
}