#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/basic_socket_types.hpp"
#include "camera.h"
#include "light.h"
#include "pxr/imaging/hd/tokens.h"
#include "render_node_base.h"
#include "resource_allocator_instance.hpp"
#include "rich_type_buffer.hpp"

namespace USTC_CG::node_shadertoy {
float vertices[] = {
    // positions
    -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  0.0f
};

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Camera>("Camera");

    b.add_input<decl::String>("Shader").default_val("shaders/Raymarching.fs");
    b.add_output<decl::Texture>("Color");
}

static void node_exec(ExeParams params)
{
    auto cameras = params.get_input<CameraArray>("Camera");

    auto free_camera = cameras.back();
    auto size = free_camera->_dataWindow.GetSize();

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    TextureDesc texture_desc;
    texture_desc.size = size;
    texture_desc.format = HdFormatFloat32Vec4;
    auto color_texture = resource_allocator.create(texture_desc);

    auto shaderPath = params.get_input<std::string>("Shader");

    ShaderDesc shader_desc;
    shader_desc.set_vertex_path(
        std::filesystem::path(RENDER_NODES_FILES_DIR) /
        std::filesystem::path("shaders/shadertoy.vs"));

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

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    resource_allocator.destroy(shader);

    params.set_output("Color", color_texture);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Shadertoy");
    strcpy_s(ntype.id_name, "render_shadertoy");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_shadertoy
