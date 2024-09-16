#include "GCore/Components/CurveComponent.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

std::string CurveComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Curve component. "
        << "Vertices count " << get_vertices().size()
        << ". Face vertices count "
        << ".";
    return out.str();
}

GeometryComponentHandle CurveComponent::copy(Geometry* operand) const
{
    auto ret = std::make_shared<CurveComponent>(operand);
    copy_prim(curves.GetPrim(), ret->curves.GetPrim());
    pxr::UsdGeomImageable(curves).MakeInvisible();
    return ret;
}

CurveComponent::CurveComponent(Geometry* attached_operand)
    : GeometryComponent(attached_operand)
{
    scratch_buffer_path = pxr::SdfPath(
        "/scratch_buffer/curves_component_" +
        std::to_string(reinterpret_cast<long long>(this)));
    curves = pxr::UsdGeomBasisCurves::Define(
        GlobalUsdStage::global_usd_stage, scratch_buffer_path);
    pxr::UsdGeomImageable(curves).MakeInvisible();
}

USTC_CG_NAMESPACE_CLOSE_SCOPE