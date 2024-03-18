#include <memory>

#include "GCore/MeshOperand.h"
#include "SCore/SequencedComponent.h"
#include "GCore/GOperators.h"
#include "gtest/gtest.h"

TEST(SEQUENCE_IO, read)
{
    auto test = std::make_shared<USTC_CG::MeshOperand>();
    test->read_from_usd("./monkey.usda", pxr::SdfPath("/Suzanne/Suzanne"));
     
    auto sequenced = std::make_shared<USTC_CG::GOperandBase>();
    test->copy_to(sequenced);

    auto topology = sequenced->get_component<USTC_CG::MeshComponent>();

    auto sequenced_component =
        USTC_CG::SequencedComponent::make_sequenced(topology);

    for (int i = 0; i < 30; ++i)
    {
        sequenced_component->time_stamps.push_back({ i + 1.0 });

        std::shared_ptr<USTC_CG::MeshComponent> new_step =
            std::dynamic_pointer_cast<USTC_CG::MeshComponent>(
                sequenced_component->component_sequence.back()->copy(sequenced));

        for (auto&& vertex : new_step->vertices)
        {
            vertex[0] *= 1.02f;
            vertex[1] *= 1.02f;
            vertex[2] *= 0.98f;
        }
        sequenced_component->component_sequence.push_back(new_step);
    }

    sequenced->attach_component(sequenced_component);

    sequenced->detach_component(topology);

    sequenced->write_to_usd(
        "./deforming_monkey.usda",
        pxr::SdfPath("/Suzanne/Suzanne"));

    std::cout << sequenced->to_string();
}


#include <iostream>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
