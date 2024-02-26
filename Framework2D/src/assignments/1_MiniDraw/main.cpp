#include <stdexcept>

#include "window_minidraw.h"

int main()
{
    try
    {
        USTC_CG::MiniDraw w("MiniDraw");
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