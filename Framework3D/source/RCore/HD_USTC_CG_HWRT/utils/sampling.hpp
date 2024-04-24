#include "USTC_CG.h"
#include "pxr/base/gf/math.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

using namespace pxr;
inline GfVec3f CosineWeightedDirection(const GfVec2f& uniform_float, float& pdf)
{
    GfVec3f dir;
    float theta = 2.0f * M_PI * uniform_float[0];
    float eta = uniform_float[1];
    float sqrteta = sqrtf(eta);
    dir[0] = cosf(theta) * sqrteta;
    dir[1] = sinf(theta) * sqrteta;
    dir[2] = sqrtf(1.0f - eta);

    pdf = dir[2] / M_PI;
    return dir;
}

inline float GGX(float cosTheta, float alpha)
{
    float alphaSqr = alpha * alpha;
    float cosThetaSqr = cosTheta * cosTheta;
    return alphaSqr /
           (M_PI * cosThetaSqr * cosThetaSqr * (alphaSqr + cosThetaSqr) * (alphaSqr + cosThetaSqr));
}

inline GfVec3f GGXWeightedDirection(const GfVec2f& uniform_float, float roughness, float& pdf)
{
    float alpha = roughness * roughness;
    float phi = 2.0 * M_PI * uniform_float[0];
    float cosTheta =
        sqrt((1.0 - uniform_float[1]) / (1.0 + (alpha * alpha - 1.0) * uniform_float[1]));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    float x = sinTheta * cos(phi);
    float y = sinTheta * sin(phi);
    float z = cosTheta;

    pdf = GGX(cosTheta, alpha);

    return GfVec3f(x, y, z);
}

inline GfVec3f UniformSampleSphere(const GfVec2f& uniform_float, float& pdf)
{
    float theta = 2.0f * M_PI * uniform_float[0];
    float phi = acosf(2.0f * uniform_float[1] - 1.0f);
    float sin_phi = sinf(phi);

    pdf = 1.0 / 4 / M_PI;
    return GfVec3f(sin_phi * cosf(theta), sin_phi * sinf(theta), cosf(phi));
}

inline GfVec3f UniformSampleHemiSphere(const GfVec2f& uniform_float, float& pdf)
{
    auto ret = UniformSampleSphere(uniform_float, pdf);
    pdf *= 2;
    ret[2] = abs(ret[2]);
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE