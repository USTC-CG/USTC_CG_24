#include <memory>

#include "GCore/MeshOperand.h"
#include "SCore/SequencedComponent.h"
#include "GCore/GOperators.h"
#include "SCore/SpringSystemOperator.h"
#include "gtest/gtest.h"

TEST(spring_system, read)
{
    auto test = std::make_shared<USTC_CG::MeshOperand>();
    test->read_from_usd(
        "./one_piece.usda",
        pxr::SdfPath("/Suzanne_002/Suzanne_002"));

    auto spring_system = USTC_CG::CreateSpringSystemOperator();

    auto result = spring_system->execute(test);

    result->write_to_usd(
        "./spring_monkey.usda",
        pxr::SdfPath("/Suzanne/Suzanne"));
}


#include <iostream>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
