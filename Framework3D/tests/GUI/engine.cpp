#include <pxr/usd/usdGeom/cube.h>

#include <memory>

#include "GUI/node_system.h"
#include "GUI/usd_filetree.h"
#include "GUI/usdview_engine.h"
#include "GUI/window/window.h"
#include "Nodes/GlobalUsdStage.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        geonode_system = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Geometry, "GeoNodeSystem.json", "Geometric Nodes");
        render_graph_system = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Render, "RenderGraph.json", "Render Nodes");

        composition_graph = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Composition, "CompositionGraph.json", "Composition");
        file_viewer = std::make_unique<USTC_CG::UsdFileViewer>();
        file_viewer->set_stage(USTC_CG::GlobalUsdStage::global_usd_stage);

        renderer =
            std::make_shared<USTC_CG::UsdviewEngine>(USTC_CG::GlobalUsdStage::global_usd_stage);
    }

   protected:
    void Render() override
    {
        render_graph_system->draw_imgui();

        renderer->render(render_graph_system->get_tree(), render_graph_system->get_executor());
    }
    std::shared_ptr<USTC_CG::NodeSystem> composition_graph;
    std::shared_ptr<USTC_CG::NodeSystem> geonode_system;
    std::shared_ptr<USTC_CG::NodeSystem> render_graph_system;
    std::unique_ptr<USTC_CG::UsdFileViewer> file_viewer;
    std::shared_ptr<USTC_CG::UsdviewEngine> renderer;
    void BuildUI() override;
};

void NodeWindow::BuildUI()
{
    composition_graph->draw_imgui();
    geonode_system->draw_imgui();
    file_viewer->ShowFileTree();
    file_viewer->ShowPrimInfo();
}

int main()
{
    NodeWindow w("USTC CG 2024");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
