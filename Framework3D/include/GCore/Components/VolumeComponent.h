#pragma once

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class USTC_CG_API VolumeComponet : public GeometryComponent {
   public:
    explicit VolumeComponet(Geometry* attached_operand) : GeometryComponent(attached_operand)
    {
    }

    GeometryComponentHandle copy(Geometry* operand) const override;
    std::string to_string() const override;
};
USTC_CG_NAMESPACE_CLOSE_SCOPE
