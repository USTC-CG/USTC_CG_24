#include "GCore/Components/CurveComponent.h"

USTC_CG_NAMESPACE_OPEN_SCOPE

std::string CurveComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Points component. "
        << "Vertices count " << vertices.size() << ". Face vertices count "
        << ".";
    return out.str();
}

GeometryComponentHandle CurveComponent::copy(Geometry* operand) const
{
    auto ret = std::make_shared<CurveComponent>(operand);

    // This is fast because the VtArray has the copy on write mechanism
    ret->vertices = this->vertices;
    ret->width = this->width;
    ret->displayColor = this->displayColor;
    return ret;
}

CurveComponent::CurveComponent(Geometry* attached_operand) : GeometryComponent(attached_operand)
{
}

USTC_CG_NAMESPACE_CLOSE_SCOPE