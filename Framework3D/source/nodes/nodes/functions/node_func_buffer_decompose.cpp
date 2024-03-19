#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "func_node_base.h"

namespace USTC_CG::node_buffer_decompose {
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

#define NodeDeclare(type, suffix, size)                                                \
    static void node_declare_decompose_buffer##size##suffix(NodeDeclarationBuilder& b) \
    {                                                                                  \
        b.add_input<decl::type##size##Buffer>("Buffer");                               \
        for (int i = 0; i < size; ++i) {                                               \
            b.add_output<decl::type##1Buffer>(socket_name(i).c_str());                 \
        }                                                                              \
        b.add_output<decl::Int>("Size");                                               \
    }

NodeDeclare(Float, f, 2);
NodeDeclare(Float, f, 3);
NodeDeclare(Float, f, 4);

NodeDeclare(Int, i, 2);
NodeDeclare(Int, i, 3);
NodeDeclare(Int, i, 4);

#define NodeExec(type, base_type, suffix, s)                            \
    static void node_exec_decompose_buffer##s##suffix(ExeParams params) \
    {                                                                   \
        pxr::VtArray<base_type> val[s];                                 \
        auto input = params.get_input<pxr::VtArray<type>>("Buffer");    \
        for (int i = 0; i < s; ++i) {                                   \
            val[i].resize(input.size());                                \
        }                                                               \
        for (int idx = 0; idx < input.size(); ++idx) {                  \
            for (int i = 0; i < s; ++i) {                               \
                val[i][idx] = input[idx][i];                            \
            }                                                           \
        }                                                               \
        for (int i = 0; i < s; ++i) {                                   \
            params.set_output(socket_name(i).c_str(), val[i]);          \
        }                                                               \
        params.set_output("Size", int(input.size()));                   \
    };
NodeExec(pxr::GfVec2f, float, f, 2);
NodeExec(pxr::GfVec3f, float, f, 3);
NodeExec(pxr::GfVec4f, float, f, 4);

NodeExec(pxr::GfVec2i, int, i, 2);
NodeExec(pxr::GfVec3i, int, i, 3);
NodeExec(pxr::GfVec4i, int, i, 4);

static void node_register()
{
#define NodeRegister(size, suffix)                                                       \
    static NodeTypeInfo ntype_buffer##size##suffix;                                      \
                                                                                         \
    strcpy(ntype_buffer##size##suffix.ui_name, "Decompose Buffer" #size #suffix);        \
    strcpy_s(ntype_buffer##size##suffix.id_name, "func_buffer_decompose" #size #suffix); \
                                                                                         \
    func_node_type_base(&ntype_buffer##size##suffix);                                     \
    ntype_buffer##size##suffix.node_execute = node_exec_decompose_buffer##size##suffix;  \
    ntype_buffer##size##suffix.declare = node_declare_decompose_buffer##size##suffix;    \
    nodeRegisterType(&ntype_buffer##size##suffix);

    NodeRegister(2, f);
    NodeRegister(3, f);
    NodeRegister(4, f);

    NodeRegister(2, i);
    NodeRegister(3, i);
    NodeRegister(4, i);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_buffer_decompose
