#include <boost/optional/optional_io.hpp>
#include <boost/python.hpp>
#include <boost/python/import.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/numpy/ndarray.hpp>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/buffer_socket_types.hpp"
#include "func_node_base.h"
#include "entt/meta/resolve.hpp"
#include "pxr/base/vt/arrayPyBuffer.h"

namespace USTC_CG::node_python_script {
namespace bp = boost::python;
namespace bpn = boost::python::numpy;

static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float1Buffer>("Buffer");
    b.add_input<decl::String>("Script Name").default_val("add");

    b.add_output<decl::Float1Buffer>("Buffer");
}

static void node_exec(ExeParams params)
{
    namespace bp = boost::python;
    namespace bpn = boost::python::numpy;

    auto input = params.get_input<pxr::VtArray<float>>("Buffer");
    auto script = params.get_input<std::string>("Script Name");

    try {
        // Import the Python module
        bp::object reload = bp::import("importlib").attr("reload");
        bp::object module = bp::import(script.c_str());
        module = reload(module);

        // Call the 'process_array' function from the module
        bp::object result = (module.attr("process_array")(input));
        bpn::ndarray np_arr = bpn::array(result).copy();
        std::string err;
        boost::optional<pxr::VtArray<float>> vt_arr =
            pxr::VtArrayFromPyBuffer<float>(pxr::TfPyObjWrapper(result));

        std::cout << "C++ side:" << vt_arr << std::endl;

        std::cout << "C++ numpy side: ";
        for (int i = 0; i < 5; ++i) {
            auto val = ((float*)(np_arr.get_data()))[i];
            bp::list l;

            std::cout << val << ' ';
        }
        std::cout << std::endl;
    }
    catch (const bp::error_already_set&) {
        PyErr_Print();
        throw std::runtime_error("Python error.");
    }
}

#define DEFINE_PYTHON_SCRIPT_EXEC(script)                                     \
    static void node_exec(ExeParams params)                                   \
    {                                                                         \
        auto input = params.get_input<pxr::VtArray<float>>("Buffer");         \
        auto script = params.get_input<std::string>("Script Name");           \
                                                                              \
        try {                                                                 \
            bp::object reload = bp::import("importlib").attr("reload");       \
            bp::object module = bp::import(script.c_str());                   \
            module = reload(module);                                          \
                                                                              \
            bp::object result = (module.attr("process_array")(input));        \
            bpn::ndarray np_arr = bpn::array(result).copy();                  \
            std::string err;                                                  \
            boost::optional<pxr::VtArray<float>> vt_arr =                     \
                pxr::VtArrayFromPyBuffer<float>(pxr::TfPyObjWrapper(result)); \
                                                                              \
            std::cout << "C++ side:" << vt_arr << std::endl;                  \
                                                                              \
            std::cout << "C++ numpy side: ";                                  \
            for (int i = 0; i < 5; ++i) {                                     \
                auto val = ((float*)(np_arr.get_data()))[i];                  \
                bp::list l;                                                   \
                                                                              \
                std::cout << val << ' ';                                      \
            }                                                                 \
            std::cout << std::endl;                                           \
        }                                                                     \
        catch (const bp::error_already_set&) {                                \
            PyErr_Print();                                                    \
            throw std::runtime_error("Python error.");                        \
        }                                                                     \
    }

#define DECLARE_PYTHON_SCRIPT(script)                                                \
    static void node_declare_##script(NodeDeclarationBuilder& b)                     \
    {                                                                                \
        try {                                                                        \
            bp::object module = bp::import(#script);                                 \
            bp::object declare_node_info = module.attr("declare_node")();            \
                                                                                     \
            auto list = bp::list(declare_node_info);                                 \
            auto input = bp::list(list[0]);                                          \
            auto output = bp::list(list[1]);                                         \
                                                                                     \
            for (int i = 0; i < len(input); ++i) {                                   \
                std::string s = bp::extract<std::string>(input[i]);                  \
                b.add_input<decl::Any>(s.c_str());                                   \
            }                                                                        \
            for (int i = 0; i < len(output); ++i) {                                  \
                std::string s = bp::extract<std::string>(output[i]);                 \
                b.add_output<decl::Any>(s.c_str());                                  \
            }                                                                        \
        }                                                                            \
        catch (const bp::error_already_set&) {                                       \
            PyErr_Print();                                                           \
            throw std::runtime_error("Python error. Node delare fails in " #script); \
        }                                                                            \
    };

DECLARE_PYTHON_SCRIPT(add)

static void node_exec_add(ExeParams params)
{
    try {
        bp::object reload = bp::import("importlib").attr("reload");
        bp::object module = bp::import("add");

        bp::object declare_node_info = module.attr("declare_node")();
        auto list = bp::list(declare_node_info);
        auto input = bp::list(list[0]);
        auto output = bp::list(list[1]);

        bp::list input_l;
        for (int i = 0; i < len(input); ++i) {
            std::string s = bp::extract<std::string>(input[i]);
            auto storage = params.get_input<entt::meta_any>(s.c_str());
            if (storage.type()==entt::resolve<float>()) {
                float val = storage.cast<float>();
                input_l.append(val);
            }
        }

        module = reload(module);
        bp::object result = module.attr("wrap_exec")(input);

        if (len(output) > 1) {

        }
        else if (len(output) == 1) {
            auto extract_float = bp::extract<float>(result);
            if(extract_float.check()) {
            }

                
        }
        bpn::ndarray np_arr = bpn::array(result).copy();
        std::string err;
        boost::optional<pxr::VtArray<float>> vt_arr =
            pxr::VtArrayFromPyBuffer<float>(pxr::TfPyObjWrapper(result));
        std::cout << "C++ side:" << vt_arr << std::endl;
        std::cout << "C++ numpy side: ";
        for (int i = 0; i < 5; ++i) {
            auto val = ((float*)(np_arr.get_data()))[i];
            bp::list l;
            std::cout << val << ' ';
        }
        std::cout << std::endl;
    }
    catch (const bp::error_already_set&) {
        PyErr_Print();
        throw std::runtime_error("Python error.");
    }
};

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Python Script");
    strcpy(ntype.id_name, "geom_python_script");

    func_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare_add;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_python_script
