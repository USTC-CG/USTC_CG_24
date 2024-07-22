#pragma once

#include <memory>

#include "GCore/Components.h"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class USTC_CG_API SequencedComponent : public GeometryComponent {
   public:
    explicit SequencedComponent(Geometry* attached_operand)
        : GeometryComponent(attached_operand)
    {
    }

    std::string to_string() const override;
    GeometryComponentHandle copy(Geometry* operand) const override;

    std::vector<pxr::UsdTimeCode> time_stamps;
    std::vector<GeometryComponentHandle> component_sequence;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
