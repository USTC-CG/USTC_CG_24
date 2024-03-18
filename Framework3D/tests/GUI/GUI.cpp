#include <stdexcept>

#include "GUI/window/window.h"

int main()
{
    try
    {
        USTC_CG::Window w("demo");
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
