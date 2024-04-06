#include "GUI/node_system.h"

#include <memory>

#include "GUI/window/window.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        geonode_system = std::make_shared<USTC_CG::NodeSystem>(
            USTC_CG::NodeSystemType::Geometry, "Blueprints.json", window_name);
    }

   protected:
    void Render() override
    {
    }
    std::shared_ptr<USTC_CG::NodeSystem> geonode_system;
    void BuildUI() override;
};

void NodeWindow::BuildUI()
{
    geonode_system->draw_imgui();
}

int main()
{
    NodeWindow w("demo");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
