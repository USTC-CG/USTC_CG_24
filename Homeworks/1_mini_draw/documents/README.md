# 作业说明文档

本次作业的说明文档主要由以下几个部分构成

- [GUI 简介](gui_introduction.md) 简要介绍了图形用户界面（GUI）以及其常用的开发工具，通过它你将对 GUI 开发以及我们接下来几次作业用到的 [Dear ImGui 库](https://github.com/ocornut/imgui) 有大致的了解。 
- [ImGui 程序框架配置说明](framework_introduction.md) 简要介绍了我们本次的作业框架，通过它你将能够配置并运行一个简单的 Demo 项目，包含简单的绘图功能（本次作业可以参考）和图像读取功能（后面作业将会用到）。
- [ImGui 程序框架具体实现](framework_details.md) 描述了作业框架中的文件是如何创建和编写的，以便同学们参考学习。

## 学习过程

- 大致阅读 [GUI 简介](gui_introduction.md)，不完全懂也没关系，不用担心！
- 根据 [ImGui 程序框架配置说明](framework_introduction.md) 配置作业项目代码，把 Demo 程序跑通，尝试其中的绘图和图像读取功能。
- 模仿 [ImGui 程序框架具体实现](framework_details.md) 中的示范，一步一步完成画**直线**和**矩形**的 MiniDraw 程序，学习如何添加按钮、处理鼠标交互等。
- 模仿上述过程，添加更多图形的绘制，如 `Ellipse`，`Polygon`，`Freehand`（自由绘制模式，optional）等。

## 补充材料和提示

### Qt 版本的 MiniDraw 实现

可以参考 [2020年图形学 MiniDraw](https://github.com/Ubpa/USTC_CG/tree/master/Homeworks/1_MiniDraw/documents) 的说明文档，其中包含了 GUI 开发工具 Qt 的详细介绍，以及使用 Qt 开发 MiniDraw 应用的教程和[示例](https://pan.baidu.com/s/1o8nXdwA)。感兴趣的同学可以通过其中的资料自行了解 Qt 的使用，比较 Qt 和本次作业框架的异同。

**注意本次作业没有使用 Qt，因此无需下载和配置 Qt 相关环境。**

### 提示：如何学习 ImGui 编程？

ImGui 包含了详细的[使用文档](https://github.com/ocornut/imgui/wiki) 和 [讨论社区](https://github.com/ocornut/imgui/issues) 以及许多第三方网站的教程。各种功能（窗口、菜单、工具栏、多窗口、图像读写、鼠标响应……）都可以找到对应的方式去实现。

注意：学习 ImGui 的方法是先模仿着用好，经验多了就会了。不要像学习数学那样，把介绍 ImGui 的文档看完后才来做。而是先模仿，用到什么再去学什么。这是学编程和学数学很大不一样的地方。

对于一个特定的功能，比如加菜单，或画图，你只要先跟着程序框架中的范例或者 ImGui 的 Demo 教程模仿一遍，运行着试试看发生什么，以后模仿着做就会了。

完成这个任务，你只需：

- 阅读 ImGui 自带简单例子的源码；
- 了解如何产生一个窗口程序，如何加菜单，加按钮；
- 按照范例学会如何交互鼠标，如何利用画图；
- 完成作业。

（有些教程中有类似于这个作业的画图工具的示范，模仿者学会加强即可。其他部分可以暂时不学）

再次重申，没有必要完全搞清楚 ImGui 才能做该作业。你只要完全跟着 ImGui 的教程模仿几个基本操作的实现（窗口、菜单、按钮、鼠标响应即可），或参考 [ImGui 程序框架配置说明](framework_introduction.md) 和 [ImGui 程序框架具体实现](framework_details.md) ，然后根据要求完成作业任务即可。

现在不要太深入的学习 ImGui 的功能！如果你过早去看 ImGui 的其他功能，你会觉得 ImGui 很难学。但事实上，没那么难。你觉得难只是你的方法有问题！就像学习游泳，你看书是永远学不会的，只有多下水多折腾，就慢慢体会，慢慢学会了。

记住：**学习=模仿+重复！**