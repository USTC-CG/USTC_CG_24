#include "gtest/gtest.h"

#include "GCore/GOP.h"
#include "GCore/VolumeComponent.h"

TEST(USTC_CG_MeshIO, read)
{
    auto test = std::make_shared<USTC_CG::GOperandBase>();

    auto volume = std::make_shared<USTC_CG::VolumeComponet>(test.get());


    test->attach_component(volume);

    std::cout << test->to_string();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
