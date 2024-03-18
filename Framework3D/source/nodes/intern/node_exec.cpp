#include "Nodes/node_exec.hpp"

#include "Nodes/node.hpp"
#include "Nodes/pin.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
int ExeParams::get_input_index(const char* identifier) const
{
    int counter = 0;
    for (NodeSocket* socket : node_.inputs) {
        if (std::string(socket->identifier) == identifier) {
            return counter;
        }
        counter++;
    }
    assert(false);
    return -1;
}

int ExeParams::get_output_index(const char* identifier)
{
    int counter = 0;
    for (NodeSocket* socket : node_.outputs) {
        if (std::string(socket->identifier) == identifier) {
            return counter;
        }
        counter++;
    }
    // If code runs here, please check whether your get_input/set_output
    // identifier match with your declaration
    assert(false);
    return -1;
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
