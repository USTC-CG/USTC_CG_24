#include <fstream>

#include "Nodes/GlobalUsdStage.h"
#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "geom_node_base.h"

namespace USTC_CG::node_print_debug_info {
static void node_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Any>("Variable");
}

#define TypesToPrint float, VtArray<float>

static void node_exec(ExeParams params)
{
    entt::meta_any storage = params.get_input<entt::meta_any>("Variable");
    using namespace pxr;
#define PrintType(type)                                 \
    if (storage.allow_cast<type>()) {                   \
        std::cout << storage.cast<type>() << std::endl; \
    }
    MACRO_MAP(PrintType, TypesToPrint)


    auto& stage = GlobalUsdStage::global_usd_stage;
    std::string str;
    stage->ExportToString(&str);

    std::ofstream out("current_stage.usda");
    out << str << std::endl;
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Debug Info");
    strcpy(ntype.id_name, "geom_print_debug_info");

    geo_node_type_base(&ntype);
    ntype.node_execute = node_exec;
    ntype.declare = node_declare;
    ntype.ALWAYS_REQUIRED = true;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_print_debug_info
