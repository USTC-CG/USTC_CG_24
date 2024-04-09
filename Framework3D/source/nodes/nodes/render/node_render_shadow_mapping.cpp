#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/basic_socket_types.hpp"
#include "camera.h"
#include "geometries/mesh.h"
#include "light.h"
#include "pxr/base/gf/frustum.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/hd/tokens.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "rich_type_buffer.hpp"
#include "utils/draw_fullscreen.h"

namespace USTC_CG::node_shadow_mapping {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Meshes>("Meshes");
    b.add_input<decl::Lights>("Lights");
    b.add_input<decl::Int>("resolution").default_val(1024).min(256).max(4096);
    b.add_input<decl::String>("Shader").default_val("shaders/shadow_mapping.fs");

    b.add_output<decl::Texture>("Shadow Maps");
}

static void node_exec(ExeParams params)
{
    auto meshes = params.get_input<MeshArray>("Meshes");
    auto lights = params.get_input<LightArray>("Lights");
    auto resolution = params.get_input<int>("resolution");

    TextureDesc texture_desc;
    texture_desc.array_size = lights.size();
    //texture_desc.array_size = 1;
    texture_desc.size = GfVec2i(resolution);
    texture_desc.format = HdFormatUNorm8Vec4;
    auto shadow_map_texture = resource_allocator.create(texture_desc);

    auto shaderPath = params.get_input<std::string>("Shader");

    ShaderDesc shader_desc;
    shader_desc.set_vertex_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/shadow_mapping.vs"));

    shader_desc.set_fragment_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) / std::filesystem::path(shaderPath));
    auto shader_handle = resource_allocator.create(shader_desc);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    std::vector<TextureHandle> depth_textures;
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glViewport(0, 0, resolution, resolution);

    for (int light_id = 0; light_id < lights.size(); ++light_id) {
        shader_handle->shader.use();

        if (!lights[light_id]->GetId().IsEmpty()) {
            GlfSimpleLight light_params =
                lights[light_id]->Get(HdTokens->params).Get<GlfSimpleLight>();
            GfVec3f light_position = { light_params.GetPosition()[0],
                                       light_params.GetPosition()[1],
                                       light_params.GetPosition()[2] };
            auto light_view_mat =
                GfMatrix4f().SetLookAt(light_position, GfVec3f(0, 0, 0), GfVec3f(0, 0, 1));

            // HW6: The matrices for lights information is here! Current value is set that "it just works". However, you should try to modify the values to see how it affects the performance of the shadow maps.
            GfFrustum frustum;
            frustum.SetPerspective(120.f, 1.0, 1, 25.f);
            auto light_projection_mat = frustum.ComputeProjectionMatrix();
            shader_handle->shader.setMat4("light_view", light_view_mat);
            shader_handle->shader.setMat4("light_projection", GfMatrix4f(light_projection_mat));

             glFramebufferTextureLayer(
                 GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadow_map_texture->texture_id, 0,
                 light_id);

            texture_desc.format = HdFormatFloat32UInt8;
            texture_desc.array_size = 1;
            auto depth_texture_for_opengl = resource_allocator.create(texture_desc);
            depth_textures.push_back(depth_texture_for_opengl);

            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_STENCIL_ATTACHMENT,
                GL_TEXTURE_2D,
                depth_texture_for_opengl->texture_id,
                0);

            glClearColor(0.f, 0.f, 0.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            for (int mesh_id = 0; mesh_id < meshes.size(); ++mesh_id) {
                auto mesh = meshes[mesh_id];

                shader_handle->shader.setMat4("model", mesh->transform);

                mesh->RefreshGLBuffer();

                glBindVertexArray(mesh->VAO);
                glDrawElements(
                    GL_TRIANGLES,
                    static_cast<unsigned int>(mesh->triangulatedIndices.size() * 3),
                    GL_UNSIGNED_INT,
                    0);
                glBindVertexArray(0);
            }
        }
    }

    for (auto&& depth_texture : depth_textures) {
        resource_allocator.destroy(depth_texture);
    }

    resource_allocator.destroy(shader_handle);
    glDeleteFramebuffers(1, &framebuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto shader_error = shader_handle->shader.get_error();

    params.set_output("Shadow Maps", shadow_map_texture);
    if (!shader_error.empty()) {
        throw std::runtime_error(shader_error);
    }
}
static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Shadow Mapping");
    strcpy_s(ntype.id_name, "render_shadow_mapping");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_shadow_mapping
