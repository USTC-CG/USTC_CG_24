#include "Nodes/node_exec.hpp"

#include "Nodes/node.hpp"
#include "Nodes/pin.hpp"

USTC_CG_NAMESPACE_OPEN_SCOPE
int ExeParams::get_input_index(const char* identifier) const
{
    return node_.find_socket_id(identifier, PinKind::Input);
}

int ExeParams::get_output_index(const char* identifier)
{
    return node_.find_socket_id(identifier, PinKind::Output);
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
