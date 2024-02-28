# ImGui 程序框架配置说明

## 文件说明

项目目录 [Framework2D/](../../../Framework2D/)，其中包含以下子目录：
- [third_party/](../../../Framework2D/third_party/)：第三方依赖库
    - [imgui/](../../../Framework2D/third_party/imgui/)：跨平台C++图形界面库 [Dear ImGui](https://github.com/ocornut/imgui)
    - [ImGuiFileDialog/](../../../Framework2D/third_party/ImGuiFileDialog/)：ImGui 的[文件窗口实现](https://github.com/aiekick/ImGuiFileDialog)
    - OpenGL 内核相关库：[glfw/](../../../Framework2D/third_party/glfw/)、[glad/](../../../Framework2D/third_party/glad/)；OpenGL（Open Graphics Library）是一个跨编程语言、跨平台的编程图形程序接口（API），它告诉硬件应该如何实现图形的绘制；而 glfw, glad 提供了 OpenGL 的窗口创建、函数调用等功能，方便人们去使用 OpenGL 来进行绘图操作。**OpenGL 在这里作为 ImGui 的渲染内核使用，绑定了 OpenGL 作为内核之后，ImGui 就可以在屏幕上绘制图形界面，我们暂时不需要关心其中的细节**
    - [stb_image.h](../../../Framework2D/third_party/stb_image.h)：图像解码库，用以从文件中读取图像
- [include/](../../../Framework2D/include/)：项目的头文件目录
    - [view/](../../../Framework2D/include/view/)：UI 视图，ImGui 的主要功能封装
- [src/](../../../Framework2D/src/)：项目的源文件目录
    - [view/](../../../Framework2D/src/view/)：UI 视图的具体实现
    - [demo/](../../../Framework2D/src/demo/)：演示程序，包括简单的绘图功能和图像显示功能
    - [assignments/](../../../Framework2D/src/assignments/)：相关作业的交互界面实现，目前只有一个子文件夹 [1_MiniDraw/](../../../Framework2D/src/assignments/1_MiniDraw/)，后续使用该框架的作业会在该目录下继续创建子项目文件夹 2_xxx/、3_xxx/ 等。

## 项目配置和 Demo 运行

程序可以编译和运行在以下环境中：
- 64 位 Windows 10/11 操作系统
- Visual Studio 2022
- CMake 版本 3.14 以上

Step 0:
如果还没有将此仓库获取到本地，请参考[简易说明](../../../Softwares/Github.md)

获取third_party中的子仓库：
```
git submodule update --init --recursive
```
如遇到网络问题可以多次执行此命令。

可以根据热身训练中的 CMake 配置教程配置本项目：

Step 1: 首先确保 VS Code 中安装了必要的 C++ 、CMake 扩展（C/C++, C/C++ Extension Pack, CMake Tools 等）。

Step 2: 在项目根目录用 VS Code 打开

```shell
> cd /your/path/to/Framework2D/ 
> code .
```
Step 3: `Ctrl+Shift+P` 打开 VS Code 命令框，选择 CMake: Select a Kit，选择工具包为 amd64

Step 4: 再次打开 VS Code 命令框，选择 CMake: Build，等待项目生成。

此时，可以看见项目的根目录下出现了三个文件夹 build/、bin/和 libs/，其中 bin/ 文件夹下有可执行文件 `demo.exe`，执行该文件可以出现以下界面

<div align=center><img width = 75% src ="figs/demo_0.png"/></div align>

其中 File > Open Image File.. 可以打开文件对话框，可选择 .jpg/.png 格式的图片显示

<div align=center><img width = 75% src ="figs/demo_0.5.png"/></div align>

Edit > Enable Canvas 选中之后，可以开启简易的直线绘制模式

<div align=center><img width = 50% src ="figs/demo_1.png"/></div align>

## 作业项目

作业项目生成在 `1_MiniDraw.exe`，其中实现了直线段`Line`和矩形线框`Rect`的绘制，单击界面上方的按钮可以修改当前绘制的类型，拖动鼠标可以实现绘制。细节请参考 [ImGui 程序框架具体实现](framework_details.md)。

<div align=center><img width = 75% src ="figs/demo_2.png"/></div align>


