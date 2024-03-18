#include "GCore/Components/VolumeComponent.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
GOperandComponentHandle VolumeComponet::copy(GOperandBase* operand) const
{
    return {};
}

std::string VolumeComponet::to_string() const
{
    return std::string(typeid(decltype(*this)).name());
}
USTC_CG_NAMESPACE_CLOSE_SCOPE
