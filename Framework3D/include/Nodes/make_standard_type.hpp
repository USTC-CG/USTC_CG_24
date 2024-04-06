#pragma once

// For all the types that have a 'simple' builder. (No min, max, default value)
#define DECLARE_SOCKET_TYPE(NAME)                                      \
    class NAME##Builder;                                               \
    class NAME## : public SocketDeclaration {                          \
       public:                                                         \
        NAME##()                                                       \
        {                                                              \
            type = SocketType::NAME;                                   \
        }                                                              \
        NodeSocket* build(NodeTree* ntree, Node* node) const override; \
        using Builder = NAME##Builder;                                 \
    };                                                                 \
    class NAME##Builder : public SocketDeclarationBuilder<NAME##> { };
