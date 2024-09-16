#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "pxr/usd/usdGeom/basisCurves.h"
#include "pxr/usd/usdGeom/curves.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API CurveComponent : public GeometryComponent {
    explicit CurveComponent(Geometry* attached_operand);

    std::string to_string() const override;

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_vertices() const
    {
        pxr::VtArray<pxr::GfVec3f> vertices;
        if (curves.GetPointsAttr())
            curves.GetPointsAttr().Get(&vertices);
        return vertices;
    }

    void set_vertices(const pxr::VtArray<pxr::GfVec3f>& vertices)
    {
        curves.CreatePointsAttr().Set(vertices);
    }

    [[nodiscard]] pxr::VtArray<float> get_width() const
    {
        pxr::VtArray<float> width;
        if (curves.GetWidthsAttr())
            curves.GetWidthsAttr().Get(&width);
        return width;
    }

    void set_width(const pxr::VtArray<float>& width)
    {
        curves.CreateWidthsAttr().Set(width);
    }

    pxr::VtArray<int> get_vert_count() const
    {
        pxr::VtArray<int> vert_count;
        if (curves.GetCurveVertexCountsAttr())
            curves.GetCurveVertexCountsAttr().Get(&vert_count);
        return vert_count;
    }

    void set_vert_count(const pxr::VtArray<int>& vert_count)
    {
        curves.CreateCurveVertexCountsAttr().Set(vert_count);
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_display_color() const
    {
        pxr::VtArray<pxr::GfVec3f> displayColor;
        if (curves.GetDisplayColorAttr())
            curves.GetDisplayColorAttr().Get(&displayColor);
        return displayColor;
    }

    void set_display_color(const pxr::VtArray<pxr::GfVec3f>& display_color)
    {
        curves.CreateDisplayColorAttr().Set(display_color);
    }

    pxr::UsdGeomBasisCurves get_usd_curve() const
    {
        return curves;
    }

   private:
    pxr::UsdGeomBasisCurves curves;

    GeometryComponentHandle copy(Geometry* operand) const override;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
