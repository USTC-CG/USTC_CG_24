#include <memory>

#include "gtest/gtest.h"
#include "GCore/MeshOperand.h"
//#include "GCore/MeshComponents.h"


using namespace USTC_CG;
TEST(USTC_CG_Mesh_Operand, read)
{
    auto operand = GOperandBase();

    auto component =std::make_shared<MeshComponent>(&operand);
    operand.attach_component(component);

    auto oprand2 = std::move(operand);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
