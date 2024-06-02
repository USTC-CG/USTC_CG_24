#include "NODES_FILES_DIR.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/render_socket_types.hpp"
#include "Nodes/socket_types/stage_socket_types.hpp"
#include "RCore/Backend.hpp"
#include "boost/python/numpy.hpp"
#include "func_node_base.h"
#include "nvrhi/utils.h"

namespace USTC_CG::node_conversion {
static void node_declare_Int_to_Float(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Int>("int");
    b.add_output<decl::Float>("float");
}

namespace bpn = boost::python::numpy;

static void node_exec_Int_to_Float(ExeParams params)
{
    auto i = params.get_input<int>("int");
    params.set_output("float", float(i));
    std::cout << "The invisible node is being executed!" << std::endl;
}

void node_exec_Texture_to_NumpyArray(ExeParams exe_params)
{
    auto t = exe_params.get_input<TextureHandle>("Texture");

    std::vector shape{ t->getDesc().width, t->getDesc().height };
    std::vector<unsigned> stride{ sizeof(float), sizeof(float) };
    void* data = nullptr;
    auto arr = bpn::from_data<std::vector<unsigned>>(
        data, bpn::dtype::get_builtin<float>(), shape, stride, boost::python::object{});

    exe_params.set_output("NumpyArray", arr);
}

void node_declare_Texture_to_NumpyArray(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Texture>("Texture");
    b.add_output<decl::NumpyArray>("NumpyArray");
}

static void node_register()
{
#define CONVERSION(name)                                           \
    static NodeTypeInfo ntype_##name;                              \
    strcpy(ntype_##name.ui_name, "invisible");                     \
    strcpy(ntype_##name.id_name, "conv_" #name);                   \
    func_node_type_base(&ntype_##name);                            \
    ntype_##name.node_execute = node_exec_##name;                  \
    ntype_##name.declare = node_declare_##name;                    \
    ntype_##name.node_type_of_grpah = NodeTypeOfGrpah::Conversion; \
    ntype_##name.INVISIBLE = true;                                 \
    nodeRegisterType(&ntype_##name);

#define CONVERSION_TYPES Int_to_Float, Texture_to_NumpyArray

    MACRO_MAP(CONVERSION, CONVERSION_TYPES)
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_conversion
