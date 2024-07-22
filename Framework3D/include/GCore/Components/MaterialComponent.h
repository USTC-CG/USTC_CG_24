#pragma once

#include <string>

#include "GCore/Components.h"
#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
struct USTC_CG_API MaterialComponent : public GeometryComponent {
    explicit MaterialComponent(Geometry* attached_operand) : GeometryComponent(attached_operand)
    {
    }

    GeometryComponentHandle copy(Geometry* operand) const override
    {
        auto ret = std::make_shared<MaterialComponent>(operand);

        // This is fast because the VtArray has the copy on write mechanism
        ret->textures = this->textures;
        return ret;
    }

    std::string to_string() const override
    {
        return {};
    }

    std::vector<std::string> textures;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
