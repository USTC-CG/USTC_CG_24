#pragma once

#include "GCore/Components.h"
#include "GCore/GOP.h"
#include "USTC_CG.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
class USTC_CG_API VolumeComponet : public GOperandComponent {
   public:
    explicit VolumeComponet(GOperandBase* attached_operand) : GOperandComponent(attached_operand)
    {
    }

    GOperandComponentHandle copy(GOperandBase* operand) const override;
    std::string to_string() const override;
};
USTC_CG_NAMESPACE_CLOSE_SCOPE
