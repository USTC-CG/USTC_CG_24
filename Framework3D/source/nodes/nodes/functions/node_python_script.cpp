#include <boost/optional/optional_io.hpp>
#include <boost/python.hpp>
#include <boost/python/import.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/numpy/ndarray.hpp>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "Nodes/socket_types/buffer_socket_types.hpp"
#include "entt/meta/resolve.hpp"
#include "func_node_base.h"
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

#define TryExraction(type)                                                    \
    auto extract_##type##_array = pxr::VtArrayFromPyBuffer<type>(result);     \
    if (extract_##type##_array.has_value()) {                                 \
        params.set_output(object_name.c_str(), extract_##type##_array.get()); \
        return;                                                               \
    }

#define ArrayTypes float, int, GfVec2f, GfVec3f, GfVec4f

static void
extract_value(ExeParams& params, const bp::object& result, const std::string& object_name)
{
    using namespace pxr;

    auto extract = bp::extract<float>(result);
    if (extract.check()) {
        float extract_float = extract;
        params.set_output(object_name.c_str(), extract_float);
        return;
    }

    MACRO_MAP(TryExraction, ArrayTypes);
}

#define InputTypes                                                                             \
    float, int, VtArray<float>, VtArray<GfVec2f>, VtArray<GfVec3f>, VtArray<GfVec4f>, GfVec2f, \
        GfVec3f, GfVec4f

static void get_inputs(bp::list& input_l, const entt::meta_any& storage)
{
#define TrySetInput(TYPE)                          \
    if (storage.type() == entt::resolve<TYPE>()) { \
        TYPE val = storage.cast<TYPE>();           \
        input_l.append(val);                       \
        return;                                    \
    }
    using namespace pxr;

    MACRO_MAP(TrySetInput, InputTypes);
}

static void node_exec_add(ExeParams params)
{
    try {
        bp::object module = bp::import("add");

        bp::object declare_node_info = module.attr("declare_node")();
        auto list = bp::list(declare_node_info);
        auto input = bp::list(list[0]);
        auto output = bp::list(list[1]);

        bp::list input_l;
        for (int i = 0; i < len(input); ++i) {
            std::string s = bp::extract<std::string>(input[i]);
            auto storage = params.get_input<entt::meta_any>(s.c_str());
            get_inputs(input_l, storage);
        }

        bp::object reload = bp::import("importlib").attr("reload");
        module = reload(module);

        bp::object result = module.attr("wrap_exec")(input_l);

        if (len(output) > 1) {
            for (int i = 0; i < len(result); ++i) {
                std::string object_name = bp::extract<std::string>(output[i]);
                extract_value(params, result[i], object_name);
            }
        }
        else if (len(output) == 1) {
            std::string object_name = bp::extract<std::string>(output[0]);
            extract_value(params, result, object_name);
        }
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
    ntype.node_execute = node_exec_add;
    ntype.declare = node_declare_add;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_python_script
