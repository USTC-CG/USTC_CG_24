#include <pxr/usd/usdGeom/cube.h>

#include <memory>

#include "GCore/GlobalUsdStage.h"
#include "GUI/node_system.h"
#include "GUI/usdview_engine.h"
#include "GUI/window/window.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        system = std::make_shared<USTC_CG::NodeSystem>();
        renderer = std::make_shared<USTC_CG::UsdviewEngine>(
            USTC_CG::GlobalUsdStage::global_usd_stage);
    }

   protected:
    void Render() override
    {
        createDockSpace(1);
        renderer->render();
        finishDockSpace();
    }

    std::shared_ptr<USTC_CG::NodeSystem> system;
    std::shared_ptr<USTC_CG::UsdviewEngine> renderer;
    void BuildUI() override;
};

void NodeWindow::BuildUI()
{
    createDockSpace(0);

    system->draw_imgui();
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
