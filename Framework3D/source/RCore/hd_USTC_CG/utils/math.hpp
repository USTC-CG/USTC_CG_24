#pragma once
#include "USTC_CG.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/vec3f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
inline GfMatrix3f constructONB(const GfVec3f& dir)
{
    auto basis = GfMatrix3f(1);

    GfVec3f xAxis;
    if (fabsf(GfDot(dir, GfVec3f(0, 0, 1))) < 0.9f) {
        xAxis = GfCross(dir, GfVec3f(0, 0, 1));
    }
    else {
        xAxis = GfCross(dir, GfVec3f(0, 1, 0));
    }
    GfVec3f yAxis = GfCross(dir, xAxis);
    basis.SetColumn(0, xAxis.GetNormalized());
    basis.SetColumn(1, yAxis.GetNormalized());
    basis.SetColumn(2, dir);

    return basis;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE