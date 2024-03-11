#include <stdexcept>

#include "window_poisson.h"

int main()
{
    try
    {
        USTC_CG::WindowPoisson w("Poisson Image Editing");
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