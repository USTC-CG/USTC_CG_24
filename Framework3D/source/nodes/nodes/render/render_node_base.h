#pragma once
#include "Nodes/node.hpp"
#include "Nodes/node_exec.hpp"
#include "Nodes/socket_types/render_socket_types.hpp"
#include "Nodes/socket_types/stage_socket_types.hpp"
#include "camera.h"
#include "rich_type_buffer.hpp"
#include "node_global_params.h"

USTC_CG_NAMESPACE_OPEN_SCOPE
inline void render_node_type_base(NodeTypeInfo* ntype)
{
    ntype->color[0] = 114 / 255.f;
    ntype->color[1] = 94 / 255.f;
    ntype->color[2] = 29 / 255.f;
    ntype->color[3] = 1.0f;

    ntype->node_type_of_grpah = NodeTypeOfGrpah::Render;
}

inline Hd_USTC_CG_Camera* get_free_camera(
    ExeParams& params,
    const std::string& camera_name = "Camera")
{
    CameraArray cameras = params.get_input<CameraArray>(camera_name.c_str());

    Hd_USTC_CG_Camera* free_camera = nullptr;
    for (auto camera : cameras) {
        if (camera->GetId() != SdfPath::EmptyPath()) {
            free_camera = camera;
            break;
        }
    }
    return free_camera;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
