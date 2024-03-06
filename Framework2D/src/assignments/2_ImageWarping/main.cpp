#include <stdexcept>

#include "window_warping.h"

int main()
{
    try
    {
        USTC_CG::ImageWarping w("Image Warping");
        if (!w.init())
            return 1;

        w.run();
        return 0;
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}