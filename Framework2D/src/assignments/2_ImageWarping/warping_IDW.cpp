#include "comp_warping.h"

namespace USTC_CG
{

std::pair<int, int> Warping_IDW::warping_f(int x, int y, int width, int height)
{
    int n = start_points_.size();
    if (n == 0)
    {
        return { x, y };
    }

    float u = 0, v = 0;
    float w = 0;
    for (int i = 0; i < n; ++i)
    {
        float dx = x - start_points_[i].x;
        float dy = y - start_points_[i].y;
        float d = dx * dx + dy * dy;
        if (d < 1e-6)
        {
            return { static_cast<int>(end_points_[i].x),
                     static_cast<int>(end_points_[i].y) };
        }
        float weight = static_cast<float>(1.0) / d;
        w += weight;
        u += weight * end_points_[i].x;
        v += weight * end_points_[i].y;
    }
    u /= w;
    v /= w;
    return { static_cast<int>(u), static_cast<int>(v) };
}
}  // namespace USTC_CG