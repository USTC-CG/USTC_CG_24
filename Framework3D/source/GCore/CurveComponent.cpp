#include "GCore/Components/CurveComponent.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

std::string CurveComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Points component. "
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
}

USTC_CG_NAMESPACE_CLOSE_SCOPE