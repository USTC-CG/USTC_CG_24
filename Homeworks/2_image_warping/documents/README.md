# 作业说明

> 旧版资料：https://pan.baidu.com/s/151e-R8tEKmI3die5rBVBAA
>
> **只须看其 demo 目录的可执行程序及操作演示** 

## 学习过程

- 根据 [ImGui 程序框架配置说明](framework_introduction.md) 配置作业项目代码，把 `2_ImageWarping` 项目跑通；
- 参考[CompWarping.cpp](../../../Framework2D/src/assignments/2_ImageWarping/comp_warping.cpp) 中的图像处理函数 `invert()`， `mirror()`，`gray_scale()` 等，学习基本的图像处理操作；
- [CompWarping.cpp](../../../Framework2D/src/assignments/2_ImageWarping/comp_warping.cpp) 中实现了一个简单的 `warping()` 函数，做到了图像的“鱼眼”变形，仿照这个函数，实现作业中的 `IDW` 和 `RBF` 图像变形方法。我们在待实现的变形方法 `warping()` 处标明了 `HW2_TODO`。具体而言，你需要：
  - 为用户界面提供选点交互（可以使用框架中实现的交互）
  - 实现两种图像变形功能，从代码的复用性以及 C++ 的 **封装**、**继承**、**多态**的角度，思考：**如何对 warping 功能进行抽象和封装更加合适？**
  - 你可以直接在目录 [2_ImageWarping/](../../../Framework2D/src/assignments/2_ImageWarping/) 下添加算法需要的 `.h` 文件和 `.cpp` 文件。

## 测试图片及报告范例

- 测试例子：须用以下 [**格子图像**](../data/test.png) 来进行测试，可以很清楚看到 warping 方法的特点

<div align=center><img width = 50% src ="../data/test.png"/></div align>

- 作业实验报告范例：[示例参考](http://pan.baidu.com/s/1i3mi2yT) 

## 其他测试图片

- 用户交互示例如下：

<div align=center><img width = 75% src ="figs/ui_demo.jpg"/></div align>

- 变形效果示例如下：

<div align=center><img width = 75% src ="figs/warp_demo.jpg"/></div align>

## 详细说明

### 面向对象编程思想

通过 [C++ 课前热身练习](../../0_cpp_warmup/) 你已经掌握了面向对象编程的基本思想（类的封装、继承、多态），其理念就是：

- **程序＝对象＋对象＋对象＋…** 

- 对象＝数据结构＋算法

这与面向过程编程（程序＝数据结构＋算法）是不一样的。

### 图像库

作为图像编程的入门，我们封装了一个 [Image](../../../Framework2D/include/view/image.h) 类提供基本的图像操作功能。

- [2_ImageWarping](../../../Framework2D/src/assignments/2_ImageWarping/) 是我们的作业项目，其中提供了一些基于 `Image` 类来操作图像的示范工程。你只要看懂 `CompImage::invert` 函数，**模仿使用 `Image` 类中的四个函数 ( `width()`, `height()`, `get_pixel()`, `set_pixel()`)** 即可操作图像的处理。不必去看其他图像处理的书籍和知识后才来处理图像编程。建议大家通过该工程来实现一个非常简单的图像算法，比如线性方法的 `Color2Gray`；
- **注意整型和浮点型的转换**，图像操作的行列下标是整型，但是在一些操作中，只有转化为浮点型运算才能保证计算的精度。

### Eigen库

- 实现RBF方法需要求解线性方程组，你可以自己实现，也可以从网上找其他程序或库来用
- 强烈推荐使用 Eigen 库来求解线性方程组，Eigen 库是强大的数学算法库，是计算机图形学必须使用的算法库
- 我们提供了 Eigen 库的使用示例：[eigen_example](eigen_example/) 

> [eigen_example](eigen_example/) 演示的添加依赖的方式重点掌握，另外为了保证项目的简洁性，不要将依赖部分加到 git 版本管理中，使用 [.gitignore](../../../.gitignore) 忽略掉 [eigen_example/src/_deps/](eigen_example/src/_deps/) 


### 补洞（Optional）

结果图像中有时会出现白色空洞或条纹，你需要分析是什么原因造成的？空洞的填补可以利用周围的已知像素进行插值填充。这也是个插值问题（即利用空洞周围一定范围的已知像素来插值该像素的颜色）。你可以尝试如何用你实现的 `IDW warping` 类（或者对 `IDW warping` 类的简单改造。）来填充这些空洞像素的颜色？

<div align=center><img width = 75% src ="figs/white_stitch.jpg"/></div align>

### ANN库（Optional）

若你需要用搜索最近点的任务（在补洞的任务中），建议学习使用如下的库：

- [Annoy(Approximate Nearest Neighbors Oh Yeah)](https://github.com/spotify/annoy)

我们提供了测试项目 [ann_example](ann_example/) 


### 注意事项

- 只须看懂英文论文中的主要思想及计算过程即可，不必追究太多背景知识及细节内容；实现基本算法即可，不必拘泥于太多细节。

- 目录不要用中文名，否则编译会出错。要习惯用英文来思考，包括代码注释等。养成使用英文的习惯！

- 务必自己独立完成该作业，做得不好没有关系，我们会指出你的问题，一步一步帮你理解该作业需要你所理解的东西，这点极其重要！只有不断从失败中改正才能有长进！我们会帮你逐步纠正错误。

