#pragma once

#include <memory>

#include "GCore/Components.h"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class USTC_CG_API SequencedComponent : public GOperandComponent {
   public:
    explicit SequencedComponent(GOperandBase* attached_operand)
        : GOperandComponent(attached_operand)
    {
    }

    std::string to_string() const override;
    GOperandComponentHandle copy(GOperandBase* operand) const override;

    std::vector<pxr::UsdTimeCode> time_stamps;
    std::vector<GOperandComponentHandle> component_sequence;
};

USTC_CG_NAMESPACE_CLOSE_SCOPE
