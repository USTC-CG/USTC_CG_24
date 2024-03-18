#include "GUI/node_system.h"

#include <memory>

#include "GUI/window/window.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        system = std::make_shared<USTC_CG::NodeSystem>();
    }

   protected:
    void Render() override
    {
    }
    std::shared_ptr<USTC_CG::NodeSystem> system;
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
    NodeWindow w("demo");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
