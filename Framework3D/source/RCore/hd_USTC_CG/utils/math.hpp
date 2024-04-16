#pragma once
#include "USTC_CG.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/vec3f.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
inline pxr::GfMatrix3f constructONB(const pxr::GfVec3f& dir)
{
    auto basis = pxr::GfMatrix3f(1);

    pxr::GfVec3f xAxis;
    if (fabsf(GfDot(dir, pxr::GfVec3f(0, 0, 1))) < 0.9f) {
        xAxis = GfCross(dir, pxr::GfVec3f(0, 0, 1));
    }
    else {
        xAxis = GfCross(dir, pxr::GfVec3f(0, 1, 0));
    }
    pxr::GfVec3f yAxis = GfCross(dir, xAxis);
    basis.SetColumn(0, xAxis.GetNormalized());
    basis.SetColumn(1, yAxis.GetNormalized());
    basis.SetColumn(2, dir);

    return basis;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE