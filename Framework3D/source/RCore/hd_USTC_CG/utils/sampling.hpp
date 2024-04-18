#pragma once
#include "USTC_CG.h"
#include "pxr/base/gf/math.h"
#include "pxr/base/gf/vec2f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

/* Division */
#ifndef M_PI_F
#define M_PI_F (3.1415926535897932f) /* pi */
#endif
#ifndef M_PI_2_F
#define M_PI_2_F (1.5707963267948966f) /* pi/2 */
#endif
#ifndef M_PI_4_F
#define M_PI_4_F (0.7853981633974830f) /* pi/4 */
#endif
#ifndef M_1_PI_F
#define M_1_PI_F (0.3183098861837067f) /* 1/pi */
#endif
#ifndef M_2_PI_F
#define M_2_PI_F (0.6366197723675813f) /* 2/pi */
#endif
#ifndef M_1_2PI_F
#define M_1_2PI_F (0.1591549430918953f) /* 1/(2*pi) */
#endif
#ifndef M_SQRT_PI_8_F
#define M_SQRT_PI_8_F (0.6266570686577501f) /* sqrt(pi/8) */
#endif
#ifndef M_LN_2PI_F
#define M_LN_2PI_F (1.8378770664093454f) /* ln(2*pi) */
#endif

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

inline float mix(float a, float b, float t)
{
    return a + t * (b - a);
}

inline float GGX(float cosTheta, float alpha)
{
    float alphaSqr = alpha * alpha;
    float cosThetaSqr = cosTheta * cosTheta;
    return alphaSqr /
           (M_PI * cosThetaSqr * cosThetaSqr * (alphaSqr + cosThetaSqr) * (alphaSqr + cosThetaSqr));
}

/* Distribute 2D uniform random samples on [0, 1] over unit disk [-1, 1], with concentric mapping
 * to better preserve stratification for some RNG sequences. */
inline GfVec2f sample_uniform_disk(const GfVec2f rand)
{
    float phi, r;
    float a = 2.0f * rand[0] - 1.0f;
    float b = 2.0f * rand[1] - 1.0f;

    if (a == 0.0f && b == 0.0f) {
        return GfVec2f(0);
    }
    else if (a * a > b * b) {
        r = a;
        phi = M_PI_4_F * (b / a);
    }
    else {
        r = b;
        phi = M_PI_2_F - M_PI_4_F * (a / b);
    }

    return GfVec2f(r * cosf(phi), r * sinf(phi));
}

inline float safe_sqrt(float f)
{
    return sqrtf(std::max(f, 0.0f));
}

#define normalize(x)   (x).GetNormalized()
#define sqr(x)         (x) * (x)
#define fmadd(x, y, z) (x) * (y) + (z)
#define fmsub(x, y, z) (x) * (y) - (z)
#define rcp(x)         1.0f / (x)

#define Float float

#define Vector3f GfVec3f

#define eq(a, b) (a) == (b)

/**
 * \brief Smith's shadowing-masking function for a single direction
 *
 * \param v
 *     An arbitrary direction
 * \param m
 *     The microfacet normal
 */
inline float smith_g1(const GfVec3f& v, const GfVec3f& m, float alpha)
{
    float xy_alpha_2 = alpha * v[0] * (alpha * v[0]) + alpha * v[1] * (alpha * v[1]);
    float tan_theta_alpha_2 = xy_alpha_2 / (v[2] * v[2]);
    float result;

    result = 2.f / (1.f + sqrt(1.f + tan_theta_alpha_2));

    // Perpendicular incidence -- no shadowing/masking
    if (xy_alpha_2 == 0.f)
        result = 1.f;

    /* Ensure consistent orientation (can't see the back
       of the microfacet from the front and vice versa) */
    if (v * m * v[2] <= 0.f)
        result = 0.f;

    return result;
}

inline float GGXEval(const GfVec3f& m, float alpha)
{
    float alpha_uv = alpha * alpha;
    float cos_theta = m[2];
    float result;
    // GGX / Trowbridge-Reitz distribution function
    result = rcp(M_PI * alpha_uv * sqr(sqr(m[0] / alpha) + sqr(m[1] / alpha) + sqr(m[2])));

    // Prevent potential numerical issues in other stages of the model
    return result * cos_theta > 1e-20f ? result : 0.f;
}

inline float GGXPdf(const GfVec3f& m, const GfVec3f& wo, float roughness)
{
    float alpha = roughness * roughness;
    return GGXEval(m, alpha) * smith_g1(wo, m, alpha) * abs(wo * m) / wo[2];
}

inline GfVec3f
GGXWeightedDirection(const GfVec3f& wo, const GfVec2f& rand, float roughness, float& pdf)
{
    float alpha = roughness * roughness;
    // Visible normal sampling.

    // Step 1: stretch wo
    GfVec3f wo_p = GfVec3f(alpha * wo[0], alpha * wo[1], wo[2]).GetNormalized();

    /* Section 4.1: Orthonormal basis. */
    float lensq = (wo_p[0]) * (wo_p[0]) + (wo_p[1]) * (wo_p[1]);
    GfVec3f T1, T2;
    if (lensq > 1e-7f) {
        T1 = GfVec3f(-wo_p[1], wo_p[0], 0.0f) / sqrtf(lensq);
        T2 = GfCross(wo_p, T1);
    }
    else {
        /* Normal incidence, any basis is fine. */
        T1 = GfVec3f(1.0f, 0.0f, 0.0f);
        T2 = GfVec3f(0.0f, 1.0f, 0.0f);
    }

    /* Section 4.2: Parameterization of the projected area. */
    auto t = sample_uniform_disk(rand);
    t[1] = mix(safe_sqrt(1.0f - t[0] * t[0]), t[1], 0.5f * (1.0f + wo_p[2]));

    /* Section 4.3: Reprojection onto hemisphere. */
    GfVec3f H_ = t[0] * T1 + t[1] * T2 + safe_sqrt(1.0f - t.GetLengthSq()) * wo_p;
    auto m = normalize(GfVec3f(alpha * H_[0], alpha * H_[1], std::max(0.0f, H_[2])));

    pdf = GGXPdf(m, wo, roughness);

    /* Section 3.4: Transforming the normal back to the ellipsoid configuration. */
    return m;
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