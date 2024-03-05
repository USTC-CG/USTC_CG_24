# Clang-uml 简略使用教程

对于非 Visual Studio 用户，生成 C++ 代码的 class diagram 可能较为困难，这篇文档将简要介绍如何安装、使用 clang-uml 工具、如何通过 plantuml 工具生成类图等，以及一些常见问题的解决方法。

## 安装

### 安装 Clang-uml

此部分内容参考：[Clang-uml 上游文档](https://github.com/bkryza/clang-uml/blob/master/docs/installation.md), 下面内容仅在 Linux (Archlinux) & macOS(arm64, Sonoma) 上进行测试。

#### Windows

[Clang-uml 上游](https://github.com/bkryza/clang-uml/) 提供了预编译好的 Windows 版本，可以直接下载使用，地址在 <https://github.com/bkryza/clang-uml/releases>

选择 `clang-uml-<version>-win64.exe` 下载即可。

#### macOS

##### 拉取代码

首先需要将 [Clang-uml](https://github.com/bkryza/clang-uml) 拉取到本地：

```bash
git clone https://github.com/bkryza/clang-uml
cd clang-uml
```

##### Homebrew

接着通过 Homebrew 安装相关依赖：

> 如果你以前从没使用过 Homebrew, 以下是一个简短的介绍 & 安装方法，已经安装的同学可以跳过这一部分：

Homebrew 是 macOS 上知名的包管理器，可以通过它能很方便地安装很多软件 & 依赖，官网是 [brew.sh](https://brew.sh)。

安装方式：

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

> [!NOTE]
> **推荐的安装方式**：
>
> 对于大部分同学，访问 GitHub 以及 Homebrew 默认的站点可能有网络上的问题，在这里推荐由 USTCLUG 维护的镜像站点，可以通过以下命令安装 Homebrew：
>
> ```bash
> export HOMEBREW_BREW_GIT_REMOTE="https://mirrors.ustc.edu.cn/brew.git"
> export HOMEBREW_CORE_GIT_REMOTE="https://mirrors.ustc.edu.cn/homebrew-core.git"
> export HOMEBREW_BOTTLE_DOMAIN="https://mirrors.ustc.edu.cn/homebrew-bottles"
> export HOMEBREW_API_DOMAIN="https://mirrors.ustc.edu.cn/homebrew-bottles/api"
>
> /bin/bash -c "$(curl -fsSL https://mirrors.ustc.edu.cn/misc/brew-install.sh)"
> ```

##### 安装依赖

```bash
brew install pkg-config yaml-cpp
```

##### 编译

```bash
make release
```

##### 安装 clang-uml

```bash
sudo make install
```

#### Linux

[Clang-uml 上游](https://github.com/bkryza/clang-uml/) 对使用 `Ubuntu`, `Fedora`, `Conda`的用户已经提供了对应的文档，请直接参考<https://github.com/bkryza/clang-uml/blob/master/docs/installation.md>

### 安装 PlantUML

PlantUML 是一个开源的项目，可以通过它绘制 UML 图，官网是 [plantuml.com](https://plantuml.com/)

#### Windows

在 Windows 上安装 PlantUML 可以通过 Chocolatey 安装：

> [!NOTE]
>
> `Chocolatey` 是 Windows 上的包管理器，类似于 macOS 上的 Homebrew，官网是 [chocolatey.org](https://chocolatey.org/), 安装方法参考：<https://chocolatey.org/install>

```powershell
choco install plantuml
```

#### macOS

在 macOS 上安装 PlantUML 可以通过 Homebrew 安装：

```bash
brew install plantuml
```

#### Linux

在 Linux 上安装 PlantUML 可以通过包管理器安装：

```bash
sudo apt install plantuml # Ubuntu
sudo dnf install plantuml # Fedora
yay -S plantuml # Archlinux / Manjaro
```

### 使用

#### 生成类图

```bash
clang-uml
```

#### 输出到图片

```bash
plantuml ./diagram/1_MiniDraw.uml
```
