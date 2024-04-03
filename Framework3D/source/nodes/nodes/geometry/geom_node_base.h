#pragma once
#include "Nodes/socket_types/geo_socket_types.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
inline void geo_node_type_base(NodeTypeInfo* ntype)
{
    ntype->color[0] = 29 / 255.f;
    ntype->color[1] = 114 / 255.f;
    ntype->color[2] = 94 / 255.f;
    ntype->color[3] = 1.0f;

    ntype->node_type_of_grpah = NodeTypeOfGrpah::Geometry;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
