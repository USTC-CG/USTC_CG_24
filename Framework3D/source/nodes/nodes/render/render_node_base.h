#pragma once
#include "rich_type_buffer.hpp"
#include "Nodes/node.hpp"
#include "Nodes/socket_types/render_socket_types.hpp"
#include "camera.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
inline void render_node_type_base(NodeTypeInfo* ntype)
{
    ntype->color[0] = 114 / 255.f;
    ntype->color[1] = 94 / 255.f;
    ntype->color[2] = 29 / 255.f;
    ntype->color[3] = 1.0f;

    ntype->node_type_of_grpah = NodeTypeOfGrpah::Render;
}

inline Hd_USTC_CG_Camera* get_free_camera(ExeParams& params)
{
    auto cameras = params.get_input<CameraArray>("Camera");

    Hd_USTC_CG_Camera* free_camera;
    for (auto camera : cameras) {
        if (camera->GetId() != SdfPath::EmptyPath()) {
            free_camera = camera;
            break;
        }
    }
    return free_camera;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
