// #undef _MSC_VER

#include "GCore/Components/PointsComponent.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::string PointsComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Points component. "
        << "Vertices count " << vertices.size() << ". Face vertices count "
        << ".";
    return out.str();
}

GOperandComponentHandle PointsComponent::copy(GOperandBase* operand) const
{
    auto ret = std::make_shared<PointsComponent>(operand);

    // This is fast because the VtArray has the copy on write mechanism
    ret->vertices = this->vertices;
    ret->width = this->width;
    ret->displayColor = this->displayColor;
    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
