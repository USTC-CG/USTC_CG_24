#include <boost/python.hpp>
#include <boost/python/import.hpp>
#include <boost/python/numpy/ndarray.hpp>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/buffer_socket_types.hpp"
#include "Nodes/socket_types/stage_socket_types.hpp"
#include "RCore/Backend.hpp"
#include "entt/meta/resolve.hpp"
#include "pxr/base/vt/arrayPyBuffer.h"
#include "render_node_base.h"

#define LOAD_MODULE(name)                                       \
    bp::object module = bp::import(#name);                      \
    bp::object reload = bp::import("importlib").attr("reload"); \
    module = reload(module);

namespace USTC_CG::node_render_rayleigh_sommerfeld {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::NumpyArray>("Input");
    b.add_output<decl::NumpyArray>("Output");
}

static void node_exec(ExeParams params)
{
    auto arr = params.get_input<nparray>("Input");

    namespace bp = boost::python;

    try {
        LOAD_MODULE(rayleight_sommerfeld)

        bp::object obj = module.attr("compute")(arr);
        bp::numpy::ndarray result = bp::numpy::array{ obj };
    }
    catch (const bp::error_already_set&) {
        PyErr_Print();
        throw std::runtime_error("Python error.");
    }
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Rayleigh Sommerfeld");
    strcpy(ntype.id_name, "node_render_rayleigh_sommerfeld");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_rayleigh_sommerfeld
