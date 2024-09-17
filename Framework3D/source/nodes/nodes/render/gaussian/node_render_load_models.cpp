#include "../NODES_FILES_DIR.h"
#include "../render_node_base.h"
#include "../resource_allocator_instance.hpp"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "RCore/Backend.hpp"
#include "boost/python/numpy.hpp"
#include "nvrhi/utils.h"

namespace USTC_CG::node_render_load_models {
namespace np = boost::python::numpy;

struct Storage {
    np::ndarray xyz;
    np::ndarray opacity;
    np::ndarray trbf_center;
    np::ndarray trbf_scale;
    np::ndarray motion;
    np::ndarray features_dc;
    np::ndarray scales;
    np::ndarray rots;
    np::ndarray omegas;
    np::ndarray fts;
};
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_runtime_storage<Storage>();

    b.add_output<decl::Buffer>("xyz");
    b.add_output<decl::Buffer>("opacity");
    b.add_output<decl::Buffer>("trbf_center");
    b.add_output<decl::Buffer>("trbf_scale");
    b.add_output<decl::Buffer>("motion");
    b.add_output<decl::Buffer>("features_dc");
    b.add_output<decl::Buffer>("scales");
    b.add_output<decl::Buffer>("rots");
    b.add_output<decl::Buffer>("omegas");
    b.add_output<decl::Buffer>("fts");
}

static void node_exec(ExeParams params)
{
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "render_load_models");
    strcpy(ntype.id_name, "node_render_load_models");

    render_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_render_load_models
