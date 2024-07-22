// #undef _MSC_VER

#include "GCore/Components/PointsComponent.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::string PointsComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Points component. "
        << "Vertices count " << get_vertices().size()
        << ". Face vertices count "
        << ".";
    return out.str();
}

GeometryComponentHandle PointsComponent::copy(Geometry* operand) const
{
    auto ret = std::make_shared<PointsComponent>(operand);
    copy_prim(points.GetPrim(), ret->points.GetPrim());
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
