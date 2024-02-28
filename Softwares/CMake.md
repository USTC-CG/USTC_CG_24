# CMake 入门

CMake是一个跨平台的安装（编译）工具，可以用简单的语句来描述所有平台的安装（编译过程），开发者通过编写一种与平台无关的 CMakeList.txt 文件来定制整个编译流程，然后再根据目标用户的平台进一步生成所需的本地化 Makefile 和工程文件。CMake能够有效地描述这些文件之间的依赖关系以及处理命令，当个别文件改动后仅执行必要的处理，而不必重复整个编译过程，可以大大提高软件开发的效率。

本次作业我们推荐使用VS Code的官方CMake插件:
![](./images/cmake-plugin.png)

在使用上十分简单：用vscode打开目标文件夹（通常是有CMakeLists.txt的根目标），如果是第一次打开该文件右下角会跳出选项：“是否需要配置当前文件夹？”选择是之后会出现如下界面（该界面也可以通过使用ctrl+shift+p打开命令菜单，输入`CMake:Select a Kit`调出）：

![](./images/cmake-configure.png)

选择amd64后，使用ctrl+shift+p打开命令菜单，输入`CMake:build`,如下图：

![](./images/cmake-use.png)

CMake插件就会自动构建并编译当前项目，得到可执行文件。

## 插件选项（从作业4后开始为必须）：

1. `Ctrl+.` 打开设置
2. 搜索 cmake build type
3. 勾选 `Cmake: Set Build Type On Multi Config`选项


# CMake 使用实例

一个使用 CMake 搭建项目的实例，可以参考 [CMake step by step](../Homeworks/0_cpp_warmup/documents/CMake_step_by_step/CMAKE_step_by_step.md)。