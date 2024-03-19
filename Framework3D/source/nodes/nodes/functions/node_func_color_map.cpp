#include <algorithm>

#include "Nodes/node.hpp"
#include "Nodes/node_declare.hpp"
#include "Nodes/node_register.h"
#include "../geometry/geom_node_base.h"

namespace USTC_CG::node_func_color_map {
static void node_func_color_map_declare(NodeDeclarationBuilder& b)
{
    b.add_input<decl::Float1Buffer>("Vals");

    b.add_output<decl::Float3Buffer>("Colors");
}

static void node_func_color_map_exec(ExeParams params)
{
    auto input = params.get_input<pxr::VtArray<float>>("Vals");

    if (input.empty()) {
        throw std::runtime_error("Func Color Map: Require input data");
    }

    auto minmax = std::minmax_element(input.begin(), input.end());
    float min = *minmax.first;
    float max = *minmax.second;

    float range = max - min;

    pxr::VtArray<pxr::GfVec3f> colors(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        float normalizedValue;
        if (input[i] <= 0) {
            float minToZeroRange = 0 - min;
            normalizedValue = (input[i] - min) / minToZeroRange * 0.5;
        }
        else {
            float zeroToMaxRange = max - 0;
            normalizedValue = 0.5 + (input[i] / zeroToMaxRange) * 0.5;
        }

        pxr::GfVec3f color(1 - normalizedValue, normalizedValue, 0);
        if (normalizedValue <= 0.5) {
            color = pxr::GfVec3f(2 * (0.5 - normalizedValue), 1 - 2 * (0.5 - normalizedValue), 0);
        }
        else {
            color = pxr::GfVec3f(2 * (1 - normalizedValue), 2 * (normalizedValue - 0.5), 0);
        }
        colors[i] = color;
    }

    params.set_output("Colors", colors);
}

static void node_register()
{
    static NodeTypeInfo ntype;

    strcpy(ntype.ui_name, "Func Color Map");
    strcpy_s(ntype.id_name, "geom_func_color_map");

    ntype.node_execute = node_func_color_map_exec;
    ntype.declare = node_func_color_map_declare;
    nodeRegisterType(&ntype);
}

NOD_REGISTER_NODE(node_register)
}  // namespace USTC_CG::node_func_color_map
