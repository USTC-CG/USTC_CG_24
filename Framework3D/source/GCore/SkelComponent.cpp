// #undef _MSC_VER

#include "GCore/Components/SkelComponent.h"

#include "GCore/GOP.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
std::string SkelComponent::to_string() const
{
    std::ostringstream out;
    // Loop over the vertices and print the data
    out << "Skel component. "
        << "joint count " << topology.GetNumJoints()
        << ".";
    return out.str();
}

GOperandComponentHandle SkelComponent::copy(GOperandBase* operand) const
{
    auto ret = std::make_shared<SkelComponent>(operand);

    // This is fast because the VtArray has the copy on write mechanism
    ret->jointOrder = this->jointOrder;
    ret->topology = this->topology;
    ret->localTransforms = this->localTransforms;
    ret->bindTransforms = this->bindTransforms;
    ret->jointWeight = this->jointWeight;
    ret->jointIndices = this->jointIndices;

    return ret;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
