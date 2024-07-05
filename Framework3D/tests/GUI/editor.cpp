#include <pxr/usd/usdGeom/cube.h>

#include <filesystem>
#include <memory>

#include "GUI/node_system.h"
#include "GUI/ui_event.h"
#include "GUI/usd_filetree.h"
#include "GUI/usdview_engine.h"
#include "GUI/window/window.h"
#include "Nodes/GlobalUsdStage.h"
#include "imgui.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        if (std::filesystem::exists("stage.usda")) {
            USTC_CG::GlobalUsdStage::global_usd_stage =
                pxr::UsdStage::Open("stage.usda");
        }
        render_graph_system = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Render,
            "RenderGraph.json",
            "Render Nodes");

        composition_graph = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Composition,
            "CompositionGraph.json",
            "Composition");
        file_viewer = std::make_unique<USTC_CG::UsdFileViewer>();
        file_viewer->set_stage(USTC_CG::GlobalUsdStage::global_usd_stage);

        renderer = std::make_shared<USTC_CG::UsdviewEngine>(
            USTC_CG::GlobalUsdStage::global_usd_stage);
    }

    ~NodeWindow()
    {
        USTC_CG::GlobalUsdStage::global_usd_stage->GetRootLayer()->Export(
            "stage.usda");
    }

   protected:
    void Render() override
    {
        render_graph_system->draw_imgui();

        renderer->render(
            render_graph_system->get_tree(),
            render_graph_system->get_executor());
    }
    std::shared_ptr<USTC_CG::NodeSystem> composition_graph;
    std::vector<std::shared_ptr<USTC_CG::NodeSystem>> geonode_systems;
    std::shared_ptr<USTC_CG::NodeSystem> render_graph_system;
    std::unique_ptr<USTC_CG::UsdFileViewer> file_viewer;
    std::shared_ptr<USTC_CG::UsdviewEngine> renderer;
    void BuildUI() override;
};

void NodeWindow::BuildUI()
{
    std::unique_ptr<USTC_CG::PickEvent> pick_event = renderer->get_pick_event();
    if (pick_event) {
        USTC_CG::logging("Pick!");
    }

    composition_graph->draw_imgui();

    int removed_editor = -1;
    for (int i = 0; i < geonode_systems.size(); ++i) {
        geonode_systems[i]->consume_pickevent(pick_event.get());
        if (!geonode_systems[i]->draw_imgui()) {
            removed_editor = i;
        }
    }
    if (removed_editor >= 0) {
        assert(removed_editor < geonode_systems.size());
        geonode_systems.erase(geonode_systems.begin() + removed_editor);
    }

    float time_code_to_render = renderer->current_time_code();

    // if (time_code_to_render > geonode_system->cached_last_time_code()) {
    //     auto cached_time = geonode_system->cached_last_time_code();
    //     renderer->set_current_time_code(cached_time);

    //    geonode_system->set_required_time_code(time_code_to_render);
    //}

    file_viewer->ShowFileTree();
    file_viewer->ShowPrimInfo();

    auto new_json_path = file_viewer->emit_editor_info_path();
    if (!new_json_path.IsEmpty()) {
        auto new_geonode_system_ui = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Geometry,
            new_json_path,
            new_json_path.GetAsString());
        geonode_systems.push_back(new_geonode_system_ui);
    }

    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            USTC_CG::GlobalUsdStage::global_usd_stage->GetRootLayer()->Export(
                "stage.usda");
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("View Mode")) {
            renderer->set_edit_mode(false);
        }
        if (ImGui::MenuItem("Edit Mode")) {
            renderer->set_edit_mode(true);
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

int main()
{
    NodeWindow w("USTC CG 2024");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
