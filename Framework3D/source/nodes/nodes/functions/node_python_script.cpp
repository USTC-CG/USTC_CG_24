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

#define DECLARE_PYTHON_SCRIPT(script)                                     \
    static void node_declare_##script(NodeDeclarationBuilder& b)          \
    {                                                                     \
        try {                                                             \
            bp::object module = bp::import(#script);                      \
            bp::object declare_node_info = module.attr("declare_node")(); \
                                                                          \
            auto list = bp::list(declare_node_info);                      \
            auto input = bp::list(list[0]);                               \
            auto output = bp::list(list[1]);                              \
                                                                          \
            for (int i = 0; i < len(input); ++i) {                        \
                std::string s = bp::extract<std::string>(input[i]);       \
                b.add_input<decl::Any>(s.c_str());                        \
            }                                                             \
            for (int i = 0; i < len(output); ++i) {                       \
                std::string s = bp::extract<std::string>(output[i]);      \
                b.add_output<decl::Any>(s.c_str());                       \
            }                                                             \
        }                                                                 \
        catch (const bp::error_already_set&) {                            \
            PyErr_Print();                                                \
            throw std::runtime_error(                                     \
                "Python error. Node delare fails in " #script);           \
        }                                                                 \
    };

#define TryExraction(type)                                                    \
    auto extract_##type##_array = pxr::VtArrayFromPyBuffer<type>(result);     \
    if (extract_##type##_array.has_value()) {                                 \
        params.set_output(object_name.c_str(), extract_##type##_array.get()); \
        return;                                                               \
    }

#define ArrayTypes float, int, GfVec2f, GfVec3f, GfVec4f

static void extract_value(
    ExeParams& params,
    const bp::object& result,
    const std::string& object_name)
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

#define InputTypes                                                  \
    float, int, VtArray<float>, VtArray<GfVec2f>, VtArray<GfVec3f>, \
        VtArray<GfVec4f>, GfVec2f, GfVec3f, GfVec4f

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

#define DEFINE_PYTHON_SCRIPT_EXEC(script)                                      \
    static void node_exec_##script(ExeParams params)                           \
    {                                                                          \
        try {                                                                  \
            bp::object module = bp::import(#script);                           \
                                                                               \
            bp::object declare_node_info = module.attr("declare_node")();      \
            auto list = bp::list(declare_node_info);                           \
            auto input = bp::list(list[0]);                                    \
            auto output = bp::list(list[1]);                                   \
                                                                               \
            bp::list input_l;                                                  \
            for (int i = 0; i < len(input); ++i) {                             \
                std::string s = bp::extract<std::string>(input[i]);            \
                auto storage = params.get_input<entt::meta_any>(s.c_str());    \
                get_inputs(input_l, storage);                                  \
            }                                                                  \
                                                                               \
            bp::object reload = bp::import("importlib").attr("reload");        \
            module = reload(module);                                           \
                                                                               \
            bp::object result = module.attr("wrap_exec")(input_l);             \
                                                                               \
            if (len(output) > 1) {                                             \
                for (int i = 0; i < len(result); ++i) {                        \
                    std::string object_name =                                  \
                        bp::extract<std::string>(output[i]);                   \
                    extract_value(params, result[i], object_name);             \
                }                                                              \
            }                                                                  \
            else if (len(output) == 1) {                                       \
                std::string object_name = bp::extract<std::string>(output[0]); \
                extract_value(params, result, object_name);                    \
            }                                                                  \
        }                                                                      \
        catch (const bp::error_already_set&) {                                 \
            PyErr_Print();                                                     \
            throw std::runtime_error("Python error.");                         \
        }                                                                      \
    }

#define SCRIPT_LIST1 add
#define BUILD_SCRIPT(script)      \
    DECLARE_PYTHON_SCRIPT(script) \
    DEFINE_PYTHON_SCRIPT_EXEC(script)

MACRO_MAP(BUILD_SCRIPT, SCRIPT_LIST1)

static void node_register()
{
#define REGISTER_SCRIPT(script)                                  \
    static NodeTypeInfo ntype##script;                           \
    strcpy(ntype##script.ui_name, #script);                      \
    strcpy(ntype##script.id_name, "geom_python_script" #script); \
    func_node_type_base(&ntype##script);                         \
    ntype##script.node_execute = node_exec_##script;             \
    ntype##script.declare = node_declare_##script;               \
    nodeRegisterType(&ntype##script);

    MACRO_MAP(REGISTER_SCRIPT, SCRIPT_LIST1)
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_python_script
