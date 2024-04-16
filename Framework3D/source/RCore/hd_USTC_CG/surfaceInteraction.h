#pragma once
#include "USTC_CG.h"
#include "color.h"
#include "material.h"
#include "pxr/base/gf/matrix3f.h"
#include "utils/math.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE

class Hd_USTC_CG_Material;

class SurfaceInteraction {
   public:
    pxr::GfVec3f position;
    pxr::GfVec3f wo;
    pxr::GfVec3f geometricNormal;
    pxr::GfVec3f tangent;

    pxr::GfVec2f barycentric;
    pxr::GfVec3f shadingNormal;
    pxr::GfVec2f texcoord;

    Color Sample(pxr::GfVec3f& dir, float& pdf, const std::function<float()>& function) const;
    Color Eval(pxr::GfVec3f wi) const;
    float Pdf(pxr::GfVec3f wi, pxr::GfVec3f wo) const;

    void PrepareTransforms();
    // This is for transforming vector! It would be different for transforming points.
    pxr::GfVec3f TangentToWorld(const pxr::GfVec3f& v_tangent_space) const;
    pxr::GfVec3f WorldToTangent(const pxr::GfVec3f& v_world_space) const;
    void flipNormal();

    Hd_USTC_CG_Material* material;

   protected:
    pxr::GfMatrix3f tangentToWorld;
    pxr::GfMatrix3f worldToTangent;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
