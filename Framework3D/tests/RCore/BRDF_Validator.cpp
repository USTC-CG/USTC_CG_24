#include <iostream>
#include <random>

#include "pxr/base/gf/vec2f.h"
using namespace pxr;
#include "RCore/hd_USTC_CG/material.h"
#include "RCore/hd_USTC_CG/utils/sampling.hpp"

int main()
{
    using namespace USTC_CG;
    auto material = std::make_unique<Hd_USTC_CG_Material>(pxr::SdfPath::EmptyPath());

    auto wo = GfVec3f(1, 4, 5).GetNormalized();

    std::default_random_engine random;

    std::uniform_real_distribution<float> uniform_dist(
        0.0f, 1.0f - std::numeric_limits<float>::epsilon());
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    float spp = 50000.0f;
    Color result = Color{ 0.f };

    for (int i = 0; i < spp; ++i) {
        auto sample = GfVec2f{ uniform_float(), uniform_float() };

        float pdf;

#if 0
        auto H = GGXWeightedDirection(sample, 0.5f, pdf);
        auto wi = 2 * wo * H * H - wo;
#else
        auto wi = CosineWeightedDirection(sample, pdf);
#endif

        result += material->Eval(wi, wo, GfVec2f(0, 0)) * wo[2] / pdf / spp;
    }

    std::cout << result << std::endl;
}
