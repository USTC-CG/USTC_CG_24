#pragma once
#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "Nodes/GlobalUsdStage.h"
#include "pxr/usd/usdGeom/points.h"
#include "pxr/usd/usdGeom/xform.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API PointsComponent : public GeometryComponent {
    explicit PointsComponent(Geometry* attached_operand)
        : GeometryComponent(attached_operand)
    {
        scratch_buffer_path = pxr::SdfPath(
            "/scratch_buffer/points_component_" +
            std::to_string(reinterpret_cast<long long>(this)));
        points = pxr::UsdGeomPoints::Define(
            GlobalUsdStage::global_usd_stage, scratch_buffer_path);
        pxr::UsdGeomImageable(points).MakeInvisible();
    }

    std::string to_string() const override;

    GeometryComponentHandle copy(Geometry* operand) const override;

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_vertices() const
    {
        pxr::VtArray<pxr::GfVec3f> vertices;
        if (points.GetPointsAttr())
            points.GetPointsAttr().Get(&vertices);
        return vertices;
    }

    [[nodiscard]] pxr::VtArray<pxr::GfVec3f> get_display_color() const
    {
        pxr::VtArray<pxr::GfVec3f> displayColor;
        if (points.GetDisplayColorAttr())
            points.GetDisplayColorAttr().Get(&displayColor);
        return displayColor;
    }

    [[nodiscard]] pxr::VtArray<float> get_width() const
    {
        pxr::VtArray<float> width;
        if (points.GetWidthsAttr())
            points.GetWidthsAttr().Get(&width);
        return width;
    }

    void set_vertices(const pxr::VtArray<pxr::GfVec3f>& vertices)
    {
        points.CreatePointsAttr().Set(vertices);
    }

    void set_display_color(const pxr::VtArray<pxr::GfVec3f>& display_color)
    {
        points.CreateDisplayColorAttr().Set(display_color);
    }

    void set_width(const pxr::VtArray<float>& width)
    {
        points.CreateWidthsAttr().Set(width);
    }

   private:
    pxr::UsdGeomPoints points;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
