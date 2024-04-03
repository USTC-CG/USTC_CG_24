#include <pxr/usd/usdGeom/cube.h>

#include <memory>

#include "GUI/node_system.h"
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
        renderer =
            std::make_shared<USTC_CG::UsdviewEngine>(USTC_CG::GlobalUsdStage::global_usd_stage);
    }

   protected:
    void Render() override
    {
        createDockSpace(1);
        renderer->render(render_graph_system->get_tree());
        finishDockSpace();
    }

    std::shared_ptr<USTC_CG::NodeSystem> geonode_system;
    std::shared_ptr<USTC_CG::NodeSystem> render_graph_system;
    std::shared_ptr<USTC_CG::UsdviewEngine> renderer;
    void BuildUI() override;
};

void NodeWindow::BuildUI()
{
    createDockSpace(0);
    geonode_system->draw_imgui();
    finishDockSpace();

    createDockSpace(2);
    render_graph_system->draw_imgui();
    finishDockSpace();
}

int main()
{
    NodeWindow w("USTC CG 2024");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
