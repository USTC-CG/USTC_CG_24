# CMake step by step

热身练习是部分同学首次进行cmake配置，这里提供一个逐步的cmake配置指南。

1. 检查所需软件安装
    - git
    - CMake
    - Visual Studio (我们需要其中的MSVC以及Windows Kit)
    - Visual Studio Code
2. 检查Visual Studio Code所需插件
    - ![Pack](image.png)
    - ![Alt text](image-1.png)
3. 使用Visual Studio Code **打开文件夹**
![Alt text](image-2.png)
这里我们打开project文件夹。
![Alt text](image-3.png)
如果有安装好CMake插件会弹出
![Alt text](image-4.png)
我们选择amd64. 如果没有所示选项，可以选择Scan for kits，如果仍没有，可检查Visual Studio是否安装以及尝试重启。
4. VSCode+CMake
![Alt text](image-5.png)
我们构建的大部分操作会在CMake插件界面完成。
CMake会生成多个构建目标，其中可以通过ALL_BUILD一次构建全部目标
![Alt text](image-6.png)
也可以在完成每一个小作业后构建executables中的目标。对于每一个小作业，可以通过
![Alt text](image-7.png)
来运行。
![Alt text](image-8.png)
也可以设置为默认构建和运行目标后使用下方快捷按钮来执行。

Samples文件夹同理。

如需重新执行CMake（比如完成作业3后），可任意打开一个CMakeLists.txt，执行一次保存操作(Ctrl+S)，会重新进行一次CMake配置。