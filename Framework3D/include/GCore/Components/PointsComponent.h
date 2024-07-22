#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API PointsComponent : public GOperandComponent {
    explicit PointsComponent(GOperandBase* attached_operand)
        : GOperandComponent(attached_operand)
    {
    }

    std::string to_string() const override;

    GOperandComponentHandle copy(GOperandBase* operand) const override;

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f>& get_vertices()
    {
        return vertices;
    }

    [[nodiscard]] pxr::VtArray<float>& get_width()
    {
        return width;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f>& get_display_color()
    {
        return displayColor;
    }

   private:
    pxr::VtArray<pxr::GfVec3f> vertices;
    pxr::VtArray<float> width;
    pxr::VtArray<pxr::GfVec3f> displayColor;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
