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
namespace bp = boost::python;

struct Storage {
    Py_intptr_t shape = 1;
    np::ndarray init_zero;

    Storage()
        : init_zero(
              np::zeros(1, &shape, np::dtype::get_builtin<float>()).copy()),
          xyz(init_zero.copy()),
          opacity(init_zero.copy()),
          trbf_center(init_zero.copy()),
          trbf_scale(init_zero.copy()),
          motion(init_zero.copy()),
          features_dc(init_zero.copy()),
          scales(init_zero.copy()),
          rots(init_zero.copy()),
          omegas(init_zero.copy()),
          fts(init_zero.copy())
    {
    }

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

    bool initialized = false;
};
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_runtime_storage<Storage>();

    b.add_output<decl::NumpyArray>("xyz");
    b.add_output<decl::NumpyArray>("opacity");
    b.add_output<decl::NumpyArray>("trbf_center");
    b.add_output<decl::NumpyArray>("trbf_scale");
    b.add_output<decl::NumpyArray>("motion");
    b.add_output<decl::NumpyArray>("features_dc");
    b.add_output<decl::NumpyArray>("scales");
    b.add_output<decl::NumpyArray>("rots");
    b.add_output<decl::NumpyArray>("omegas");
    b.add_output<decl::NumpyArray>("fts");
}

static void node_exec(ExeParams params)
{
    auto& storage = params.get_runtime_storage<Storage&>();

    if (!storage.initialized) {
        bp::object m = bp::import("read_model");
        bp::object read_results = m.attr("read_model")();
        auto list = bp::list(read_results);

        storage.xyz = np::array(list[0]).copy();
        storage.opacity = np::array(list[1]).copy();
        storage.trbf_center = np::array(list[2]).copy();
        storage.trbf_scale = np::array(list[3]).copy();
        storage.motion = np::array(list[4]).copy();
        storage.features_dc = np::array(list[5]).copy();
        storage.scales = np::array(list[6]).copy();
        storage.rots = np::array(list[7]).copy();
        storage.omegas = np::array(list[8]).copy();
        storage.fts = np::array(list[9]).copy();
        storage.initialized = true;
    }

    params.set_output("xyz", storage.xyz);
    params.set_output("opacity", storage.opacity);
    params.set_output("trbf_center", storage.trbf_center);
    params.set_output("trbf_scale", storage.trbf_scale);
    params.set_output("motion", storage.motion);
    params.set_output("features_dc", storage.features_dc);
    params.set_output("scales", storage.scales);
    params.set_output("rots", storage.rots);
    params.set_output("omegas", storage.omegas);
    params.set_output("fts", storage.fts);
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
