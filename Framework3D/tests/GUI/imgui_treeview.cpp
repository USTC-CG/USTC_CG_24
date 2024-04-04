#include "GUI/usd_filetree.h"
#include "GUI/window/window.h"
#include "imgui.h"
#include "pxr/usd/usd/stage.h"

class TreeViewUSDWindow final : public USTC_CG::Window {
   public:
    pxr::UsdStageRefPtr stage;
    std::unique_ptr<USTC_CG::UsdFileViewer> file_viewer;

    explicit TreeViewUSDWindow(const std::string& window_name) : Window(window_name)
    {
        stage = pxr::UsdStage::Open("Balls.usda");

        file_viewer = std::make_unique<USTC_CG::UsdFileViewer>();
        file_viewer->set_stage(stage);
    }

   protected:
    void BuildUI() override
    {
        file_viewer->ShowFileTree();
        file_viewer->ShowPrimInfo();
    }

    void Render() override
    {
    }
};

int main()
{
    TreeViewUSDWindow w("demo");
    if (!w.init())
        return 1;

    w.run();
    return 0;
}
