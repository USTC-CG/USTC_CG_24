// #define __GNUC__

#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/basic_socket_types.hpp"
#include "camera.h"
#include "light.h"
#include "pxr/imaging/glf/simpleLight.h"
#include "pxr/imaging/hd/tokens.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "rich_type_buffer.hpp"
#include "utils/draw_fullscreen.h"

namespace USTC_CG::node_deferred_lighting {

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");
    b.add_input<decl::Lights>("Lights");

    b.add_input<decl::Texture>("Position");
    b.add_input<decl::Texture>("diffuseColor");
    b.add_input<decl::Texture>("MetallicRoughness");
    b.add_input<decl::Texture>("Normal");
    b.add_input<decl::Texture>("Shadow Maps");

    b.add_input<decl::String>("Lighting Shader").default_val("shaders/blinn_phong.fs");
    b.add_output<decl::Texture>("Color");
}

struct LightInfo {
    GfMatrix4f light_projection;
    GfMatrix4f light_view;
    GfVec3f position;
    float radius;
    GfVec3f luminance;
    int shadow_map_id;
};

static void node_exec(ExeParams params)
{
    // Fetch all the information

    auto lights = params.get_input<LightArray>("Lights");

    auto position_texture = params.get_input<TextureHandle>("Position");
    auto diffuseColor_texture = params.get_input<TextureHandle>("diffuseColor");

    auto metallic_roughness = params.get_input<TextureHandle>("MetallicRoughness");
    auto normal_texture = params.get_input<TextureHandle>("Normal");

    auto shadow_maps = params.get_input<TextureHandle>("Shadow Maps");

    auto cameras = params.get_input<CameraArray>("Camera");

    Hd_USTC_CG_Camera* free_camera;

    for (auto camera : cameras) {
        if (camera->GetId() != SdfPath::EmptyPath()) {
            free_camera = camera;
            break;
        }
    }

    // Creating output textures.
    auto size = position_texture->desc.size;
    TextureDesc color_output_desc;
    color_output_desc.format = HdFormatFloat32Vec4;
    color_output_desc.size = size;
    auto color_texture = resource_allocator.create(color_output_desc);

    unsigned int VBO, VAO;
    CreateFullScreenVAO(VAO, VBO);

    auto shaderPath = params.get_input<std::string>("Lighting Shader");

    ShaderDesc shader_desc;
    shader_desc.set_vertex_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/fullscreen.vs"));

    shader_desc.set_fragment_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) / std::filesystem::path(shaderPath));
    auto shader = resource_allocator.create(shader_desc);
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture->texture_id, 0);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->shader.use();
    shader->shader.setVec2("iResolution", size);

    shader->shader.setInt("diffuseColorSampler", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseColor_texture->texture_id);

    shader->shader.setInt("normalMapSampler", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_texture->texture_id);

    shader->shader.setInt("metallicRoughnessSampler", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic_roughness->texture_id);

    shader->shader.setInt("shadow_maps", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_maps->texture_id);

    shader->shader.setInt("position", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, position_texture->texture_id);

    GfVec3f camPos = GfMatrix4f(free_camera->GetTransform()).ExtractTranslation();
    shader->shader.setVec3("camPos", camPos);

    GLuint lightBuffer;
    glGenBuffers(1, &lightBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
    glViewport(0, 0, size[0], size[1]);
    std::vector<LightInfo> light_vector;

    for (int i = 0; i < lights.size(); ++i) {
        if (!lights[i]->GetId().IsEmpty()) {
            GlfSimpleLight light_params = lights[i]->Get(HdTokens->params).Get<GlfSimpleLight>();
            auto diffuse4 = light_params.GetDiffuse();
            pxr::GfVec3f diffuse3(diffuse4[0], diffuse4[1], diffuse4[2]);
            auto position4 = light_params.GetPosition();
            pxr::GfVec3f position3(position4[0], position4[1], position4[2]);
            light_vector.emplace_back(GfMatrix4f(), GfMatrix4f(), position3, 0.f, diffuse3, i);

            // You can add directional light here, and also the corresponding shadow map calculation
            // part.
        }
    }

    shader->shader.setInt("light_count", light_vector.size());

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        light_vector.size() * sizeof(LightInfo),
        light_vector.data(),
        GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    DestroyFullScreenVAO(VAO, VBO);

    resource_allocator.destroy(shader);
    glDeleteBuffers(1, &lightBuffer);
    glDeleteFramebuffers(1, &framebuffer);
    params.set_output("Color", color_texture);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Deferred Lighting");
    strcpy_s(ntype.id_name, "render_deferred_lighting");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_deferred_lighting
