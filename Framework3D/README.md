# USTC_CG_2024
The assignments for the computer graphics course for 2024，Spring.

# Build
首先执行git系列操作
```
git fetch
git merge upstream/main
git submodule update --init --recursive
```
然后安装下方依赖。

# Dependencies

## Windows + MSVC
强烈建议在Windows系统下使用本框架，并使用最新版MSVC进行构建和编译。

### Python 3.10.11 
[下载地址](https://www.python.org/downloads/release/python-31011/)

安装时无需勾选Debug库，需要加入path。
### 其他依赖
本框架依赖于 https://github.com/Jerry-Shen0527/USTC_CG_2024_Dependencies 。 

我们为使用Windows操作系统的同学提供了预构建的依赖库：https://rec.ustc.edu.cn/share/4d6a6e00-e04a-11ee-bd1a-2561f5e1cc93 ，因此使用Windows的同学无需clone Dependencies仓库，只需下载并执行以下步骤。

下载SDK，解压到此README所在的文件夹，执行
```
python.exe configure.py
```
最后使用编辑器/IDE打开文件夹，或cmake后打开sln文件，即配置完成。

### 可选
python依赖：PyOpenGL PySide6 PySide2 numpy

推荐使用pip安装。

## Other Platform
clone (recursive) https://github.com/Jerry-Shen0527/USTC_CG_2024_Dependencies

在该系统下执行该仓库的build.py，检查过程中报出的依赖缺失，并安装该依赖在该平台的对应版本。欢迎同学们向该仓库提交跨平台构建脚本所需的改动。

将生成的SDK文件夹转移到本目录下，并执行
```
python.exe configure.py
```

# Trouble Shoot
1. vscode在Release模式下无法找到SDK.
![alt text](images/image3.png)

2. Python version mismatch
![alt text](images/image.png)

检查Python版本是否为3.10，如果不是，将排在3.10之前的设法移除（修改环境变量，或将排在前面的临时改名为python_bak.exe）。