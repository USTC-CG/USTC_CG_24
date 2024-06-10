#include <gtest/gtest.h>

#include <iostream>

#include "Nodes/node_register.h"
#include "Nodes/node_tree.hpp"

using namespace USTC_CG;

class NODE_TREE_SOCKET_TEST : public testing::TestWithParam<int> {
   protected:
    void SetUp() override
    {
        TestWithParam<int>::SetUp();
        register_all();
    }
};

TEST_P(NODE_TREE_SOCKET_TEST, add_socket)
{
    auto tree = std::make_shared<NodeTree>();
    auto node = tree->addNode("node_dynamic_socket");
    nlohmann::json value;
    node->Serialize(value);
    std::cout << value;
}

INSTANTIATE_TEST_SUITE_P(SocketTest, NODE_TREE_SOCKET_TEST, ::testing::Values(1));