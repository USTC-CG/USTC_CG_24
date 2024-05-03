#include <Python.h>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/buffer_socket_types.hpp"
#include "func_node_base.h"


#include "pxr/base/vt/arrayPyBuffer.h"



namespace USTC_CG::node_triangulate {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float1Buffer>("Buffer");

    b.add_input<decl::String>("Script Name").default_val("foo.py");
}



static void node_exec(ExeParams params)
{
    auto input = params.get_input<pxr::VtArray<float>>("Buffer");

}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Triangulate");
    strcpy_s(ntype.id_name, "geom_Triangulate");

    func_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_triangulate
