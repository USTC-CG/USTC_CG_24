#include <memory>

#include "GUI/node_system.h"
#include "GUI/usdview_engine.h"
#include "GUI/window/window.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        geonode_system = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Geometry, "Blueprints.json", window_name);
    }

   protected:
    std::shared_ptr<USTC_CG::NodeSystem> geonode_system;
    std::shared_ptr<USTC_CG::UsdviewEngine> usdview_engine_;
    void BuildUI() override;

    void Render() override
    {
        usdview_engine_->render();
    }
};

void NodeWindow::BuildUI()
{
    geonode_system->draw_imgui();
}

int main()
{
    try {
        NodeWindow w("demo");
        if (!w.init())
            return 1;

        w.run();
        return 0;
    }
    catch (const std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}
