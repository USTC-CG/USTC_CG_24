#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "func_node_base.h"

namespace USTC_CG::node_create_buffer {
static constexpr std::string socket_name(int i)
{
    switch (i) {
        case 0: return "X";
        case 1: return "Y";
        case 2: return "Z";
        case 3: return "W";
        default: return "FF";
    }
}

#define NodeDeclare(type, suffix, size)                                                      \
    static void node_declare_create_buffer##size##suffix(NodeDeclarationBuilder& b)          \
    {                                                                                        \
        for (int i = 0; i < size; ++i) {                                                     \
            b.add_input<decl::type>(socket_name(i).c_str()).min(-10).max(10).default_val(0); \
        }                                                                                    \
        b.add_input<decl::Int>("Size").min(1).max(200).default_val(1);                       \
        b.add_output<decl::type##size##Buffer>("Buffer");                                    \
    }
NodeDeclare(Float, f, 1);
NodeDeclare(Float, f, 2);
NodeDeclare(Float, f, 3);
NodeDeclare(Float, f, 4);

static void node_declare_create_float3f(NodeDeclarationBuilder& b)
{
    for (int i = 0; i < 3; ++i) {
        b.add_input<decl::Float>(socket_name(i).c_str()).min(-10).max(10).default_val(0);
    }
    b.add_output<decl::Float3>("Buffer");
};

NodeDeclare(Int, i, 1);
NodeDeclare(Int, i, 2);
NodeDeclare(Int, i, 3);
NodeDeclare(Int, i, 4);

#define NodeExec(type, base_type, suffix, size)                         \
    static void node_exec_create_buffer##size##suffix(ExeParams params) \
    {                                                                   \
        base_type val[size];                                            \
        for (int i = 0; i < size; ++i) {                                \
            val[i] = params.get_input<float>(socket_name(i).c_str());   \
        }                                                               \
        auto s = params.get_input<int>("Size");                         \
                                                                        \
        type data;                                                      \
        memcpy(&data, val, sizeof(type));                               \
                                                                        \
        pxr::VtArray<type> arr;                                         \
        arr.resize(s, data);                                            \
        params.set_output("Buffer", arr);                               \
    }
NodeExec(float, float, f, 1);
NodeExec(pxr::GfVec2f, float, f, 2);
NodeExec(pxr::GfVec3f, float, f, 3);
NodeExec(pxr::GfVec4f, float, f, 4);

static void node_exec_create_float3f(ExeParams params)
{
    float val[3];
    for (int i = 0; i < 3; ++i) {
        val[i] = params.get_input<float>(socket_name(i).c_str());
    }
    pxr::GfVec3f data;
    memcpy(&data, val, sizeof(pxr::GfVec3f));
    params.set_output("Buffer", data);
};

NodeExec(int, int, i, 1);
NodeExec(pxr::GfVec2i, int, i, 2);
NodeExec(pxr::GfVec3i, int, i, 3);
NodeExec(pxr::GfVec4i, int, i, 4);

static void node_register()
{
#define NodeRegister(size, suffix)                                                    \
    static NodeTypeInfo ntype_buffer##size##suffix;                                   \
                                                                                      \
    strcpy(ntype_buffer##size##suffix.ui_name, "Create Buffer" #size #suffix);        \
    strcpy_s(ntype_buffer##size##suffix.id_name, "func_create_buffer" #size #suffix); \
                                                                                      \
    func_node_type_base(&ntype_buffer##size##suffix);                                  \
    ntype_buffer##size##suffix.node_execute = node_exec_create_buffer##size##suffix;  \
    ntype_buffer##size##suffix.declare = node_declare_create_buffer##size##suffix;    \
    nodeRegisterType(&ntype_buffer##size##suffix);

    NodeRegister(1, f);
    NodeRegister(2, f);
    NodeRegister(3, f);
    NodeRegister(4, f);

    static NodeTypeInfo ntype_float3f;
    strcpy(
        ntype_float3f.ui_name,
        "Create float"
        "3"
        "f");
    strcpy_s(
        ntype_float3f.id_name,
        "func_create_float"
        "3"
        "f");
    func_node_type_base(&ntype_float3f);
    ntype_float3f.node_execute = node_exec_create_float3f;
    ntype_float3f.declare = node_declare_create_float3f;
    nodeRegisterType(&ntype_float3f);
    ;


    NodeRegister(1, i);
    NodeRegister(2, i);
    NodeRegister(3, i);
    NodeRegister(4, i);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_create_buffer
