#pragma once

#include "USTC_CG.h"
#include "pin.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
namespace decl {
class LightsBuilder;

class Lights : public SocketDeclaration {
   public:
    Lights()
    {
        type = SocketType::Lights;
    }

    NodeSocket* build(NodeTree* ntree, Node* node) const override;

    using Builder = LightsBuilder;
};


class LightsBuilder : public SocketDeclarationBuilder<Lights> { };

}  // namespace decl

USTC_CG_NAMESPACE_CLOSE_SCOPE