#include <fstream>

#include "gtest/gtest.h"
#include "GCore/MeshOperand.h"

#include "monkey.inl"


TEST(USTC_CG_MeshIO, read)
{
    auto test = std::make_shared<USTC_CG::MeshOperand>();
    test->read_from_usd("./monkey.usda", pxr::SdfPath("/Suzanne/Suzanne"));
    test->write_to_usd("./monkey2.usda", pxr::SdfPath("/Suzanne/Suzanne"));

    test->read_from_usd("./monkey2.usda", pxr::SdfPath("/Suzanne/Suzanne"));
    std::cout << test->to_string();
}

TEST(USTC_CG_Xform_IO, read)
{
    auto test = std::make_shared<USTC_CG::GOperandBase>();
    test->read_from_usd("./monkey.usda", pxr::SdfPath("/Suzanne"));
    test->write_to_usd("./monkey2.usda", pxr::SdfPath("/Suzanne"));

    test->read_from_usd("./monkey2.usda", pxr::SdfPath("/Suzanne"));

    std::cout << test->to_string();
}

int main(int argc, char* argv[])
{
    auto my_string = std::string(monkey);
    std::ofstream file("monkey.usda", std::ios::out | std::ios::trunc);
    file << my_string;
    file.close();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
