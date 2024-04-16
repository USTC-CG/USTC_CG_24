#include <random>
#include <iostream>
#include "RCore/hd_USTC_CG/material.h"
#include "RCore/hd_USTC_CG/utils/sampling.hpp"
#include "pxr/base/gf/vec2f.h"

int main()
{
    using namespace USTC_CG;
    auto material = std::make_unique<Hd_USTC_CG_Material>(pxr::SdfPath::EmptyPath());

    auto wi = GfVec3f(3, 4, 5).GetNormalized();

    std::default_random_engine random;

    std::uniform_real_distribution<float> uniform_dist(
        0.0f, 1.0f - std::numeric_limits<float>::epsilon());
    std::function<float()> uniform_float = std::bind(uniform_dist, random);

    float spp = 10000.0f;
    Color result = Color{ 0.f };

    for (int i = 0; i < 10000; ++i) {
        auto sample = GfVec2f{ uniform_float(), uniform_float() };

        float pdf;
        auto wo = CosineWeightedDirection(sample, pdf);

        result += material->Eval(wi, wo, GfVec2f(0, 0)) / pdf;
    }

    std::cout << result << std::endl;
}
