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

namespace USTC_CG::node_transparent_refraction {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");

    b.add_input<decl::Texture>("Position");
    b.add_input<decl::Texture>("diffuseColor");
    b.add_input<decl::Texture>("Normal");
    b.add_input<decl::Texture>("Depth");
    b.add_input<decl::Lights>("Lights");

    b.add_input<decl::String>("Shader").default_val("shaders/transparent_refraction.fs");
    b.add_output<decl::Texture>("Color");
}

static void node_exec(ExeParams params)
{
    auto lights = params.get_input<LightArray>("Lights");
    auto baseColor = params.get_input<TextureHandle>("diffuseColor");
    auto normal_texture = params.get_input<TextureHandle>("Normal");

    Hd_USTC_CG_Dome_Light* dome_light = nullptr;
    Hd_USTC_CG_Camera* free_camera = get_free_camera(params);

    for (int i = 0; i < lights.size(); ++i) {
        auto light = lights[i];
        if (light->GetId() != SdfPath::EmptyPath()) {
            if (HdPrimTypeTokens->domeLight == light->GetLightType()) {
                dome_light = dynamic_cast<Hd_USTC_CG_Dome_Light*>(light);
                break;
            }
        }
    }

    // Search for the light with name first. If not found, fall back to anonymous lights.
    if (!dome_light) {
        for (int i = 0; i < lights.size(); ++i) {
            auto light = lights[i];
            if (HdPrimTypeTokens->domeLight == light->GetLightType()) {
                dome_light = dynamic_cast<Hd_USTC_CG_Dome_Light*>(light);
                break;
            }
        }
    }

    auto depth = params.get_input<TextureHandle>("Depth");

    auto size = baseColor->desc.size;

    unsigned int VBO, VAO;

    CreateFullScreenVAO(VAO, VBO);

    TextureDesc texture_desc;
    texture_desc.size = size;
    texture_desc.format = HdFormatFloat32Vec4;
    auto color_texture = resource_allocator.create(texture_desc);

    auto shaderPath = params.get_input<std::string>("Shader");

    ShaderDesc shader_desc;
    shader_desc.set_vertex_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/fullscreen.vs"));

    shader_desc.set_fragment_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) / std::filesystem::path(shaderPath));
    auto shader_handle = resource_allocator.create(shader_desc);
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture->texture_id, 0);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader_handle->shader.use();
    shader_handle->shader.setVec2("iResolution", size);

    unsigned id = 0;
    if (dome_light) {
        dome_light->RefreshGLBuffer();
        dome_light->BindTextures(shader_handle->shader, id);
    }

    GfVec3f camPos = GfMatrix4f(free_camera->GetTransform()).ExtractTranslation();
    shader_handle->shader.setVec3("camPos", camPos);

    shader_handle->shader.setInt("baseColor", id);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, baseColor->texture_id);
    id++;

    shader_handle->shader.setInt("depth", id);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, depth->texture_id);
    id++;

    shader_handle->shader.setInt("normalTexture", id);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, normal_texture->texture_id);
    id++;

    auto position_texture = params.get_input<TextureHandle>("Position");

    shader_handle->shader.setInt("position", id);
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, position_texture->texture_id);
    id++;

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    DestroyFullScreenVAO(VAO, VBO);

    auto shader_error = shader_handle->shader.get_error();

    resource_allocator.destroy(shader_handle);

    params.set_output("Color", color_texture);

    if (!shader_error.empty()) {
        throw std::runtime_error(shader_error);
    }
}
static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Transparent Refraction");
    strcpy_s(ntype.id_name, "render_transparent_refraction");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_transparent_refraction
