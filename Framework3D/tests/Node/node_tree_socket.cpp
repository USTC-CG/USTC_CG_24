#include <gtest/gtest.h>

#include <iostream>

#include "Nodes/node_tree.hpp"

using namespace USTC_CG;

class TestEnv: public testing::TestWithParam<int> {


    
};

TEST(NODE_TREE_SOCKET, add_socket)
{
    auto tree = std::make_shared<NodeTree>();
    auto node = tree->addNode("node_dynamic_socket");
}