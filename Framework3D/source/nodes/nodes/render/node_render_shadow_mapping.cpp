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
    texture_desc.size = GfVec2i(resolution);
    texture_desc.format = HdFormatFloat32;
    auto shadow_map_texture = resource_allocator.create(texture_desc);

    texture_desc.format = HdFormatFloat32UInt8;
    auto depth_texture_for_opengl = resource_allocator.create(texture_desc);
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

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        depth_texture_for_opengl->texture_id,
        0);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_map_texture->texture_id, 0);

    GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, resolution, resolution);
    shader_handle->shader.use();

    for (int i = 0; i < lights.size(); ++i) {
        if (!lights[i]->GetId().IsEmpty()) {
            GlfSimpleLight light_params = lights[i]->Get(HdTokens->params).Get<GlfSimpleLight>();
            GfVec3f light_position = { light_params.GetPosition()[0],
                                       light_params.GetPosition()[1],
                                       light_params.GetPosition()[2] };
            auto light_view_mat =
                GfMatrix4f().SetLookAt(light_position, GfVec3f(0, 0, 0), GfVec3f(0, 0, 1)).GetInverse();

            GfFrustum frustum;
            frustum.SetPerspective(60.f, 1.0, 1, 10.f);

            auto light_projection_mat = frustum.ComputeProjectionMatrix();

            shader_handle->shader.setMat4(
                "light_mat", GfMatrix4f(light_projection_mat) * light_view_mat);

             glFramebufferTextureLayer(
                 GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadow_map_texture->texture_id, 0, i);

            for (int i = 0; i < meshes.size(); ++i) {
                auto mesh = meshes[i];

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    resource_allocator.destroy(shader_handle);
    resource_allocator.destroy(depth_texture_for_opengl);
    glDeleteFramebuffers(1, &framebuffer);
    
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