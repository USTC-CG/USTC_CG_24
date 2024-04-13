#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

inline GfVec3f CosineWeightedDirection(const GfVec2f& uniform_float, float& pdf)
{
    GfVec3f dir;
    float theta = 2.0f * M_PI * uniform_float[0];
    float eta = uniform_float[1];
    float sqrteta = sqrtf(eta);
    dir[0] = cosf(theta) * sqrteta;
    dir[1] = sinf(theta) * sqrteta;
    dir[2] = sqrtf(1.0f - eta);

    pdf = dir[2];
    return dir;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE