#include <memory>

#include "GUI/node_system.h"
#include "GUI/window/window.h"
#include "GUI/usdview_engine.h"

class NodeWindow final : public USTC_CG::Window {
   public:
    explicit NodeWindow(const std::string& window_name) : Window(window_name)
    {
        system = std::make_shared<USTC_CG::NodeSystem>();
    }

   protected:
    std::shared_ptr<USTC_CG::NodeSystem> system;
    std::shared_ptr<USTC_CG::UsdviewEngine> usdview_engine_;
    void BuildUI() override;

    void Render() override
    {
        createDockSpace(1);

        usdview_engine_->render();
        finishDockSpace();
    }
};

void NodeWindow::BuildUI()
{
    createDockSpace(0);

    system->draw_imgui();
    finishDockSpace();
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
