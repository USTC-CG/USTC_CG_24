#include "GCore/Components/XformComponent.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
GeometryComponentHandle XformComponent::copy(Geometry* operand) const
{
    using namespace pxr;
    auto ret = std::make_shared<XformComponent>(attached_operand);
    ret->attached_operand = operand;

    ret->rotation = rotation;
    ret->translation = translation;
    ret->scale = scale;

    return ret;
}

std::string XformComponent::to_string() const
{
    return std::string("XformComponent");
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
