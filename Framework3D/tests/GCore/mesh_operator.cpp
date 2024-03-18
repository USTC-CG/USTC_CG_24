#include <memory>

#include "gtest/gtest.h"
#include "GCore/MeshOperand.h"
#include "GCore/Components.h"
#include "GCore/GOperators.h"

TEST(USTC_CG_Operator, read)
{
    auto test = std::make_shared<USTC_CG::MeshOperand>();
    test->read_from_usd("./monkey.usda", pxr::SdfPath("/Suzanne/Suzanne"));
    std::cout << test->to_string();
}

TEST(USTC_CG_Operator, triangulate)
{
    auto test = std::make_shared<USTC_CG::MeshOperand>();
    test->read_from_usd("./monkey.usda", pxr::SdfPath("/Suzanne/Suzanne"));

    auto component = CreateOpenMeshComponent(test);
    test->attach_component(component);
    std::cout << "Before: " << test->to_string() << std::endl;

    auto triangulate = USTC_CG::CreateTriangulateOperator();
    auto out = triangulate->execute(test);

    std::cout << "Output: " << out->to_string();
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
