#pragma once

#include "GCore/Components.h"
#include "USTC_CG.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
// Stores the chain of transformation

class USTC_CG_API XformComponent : public GOperandComponent {
   public:
    GOperandComponentHandle copy(GOperandBase* operand) const override;
    std::string to_string() const override;

    explicit XformComponent(GOperandBase* attached_operand) : GOperandComponent(attached_operand)
    {
    }

    std::vector<pxr::GfVec3f> translation;
    std::vector<pxr::GfVec3f> scale;
    std::vector<pxr::GfVec3f> rotation;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
