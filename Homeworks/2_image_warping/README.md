# 2. 图像变形 Image Warping

> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework2D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告 
- 递交时间：2024年3月10日（周日）晚

## 要求

- 实现（至少）两种 warping 算法
  - [Inverse distance-weighted interpolation method (IDW)](documents/0_IDW.md)
  - [Radial basis functions interpolation method (RBF)](documents/1_RBF.md)
- 巩固面向对象编程（OOP）思想
  - 巩固**抽象**：
    - 数学变换的抽象
    - 数学变换与图像无关（类的**解耦**）
  - 封装、继承、多态
- 白缝填补（optional）
- 作业实验报告范例：[示例参考](http://pan.baidu.com/s/1i3mi2yT)

## 目的

- 实现（至少）两种 warping 算法（[IDW](documents/0_IDW.md) 和 [RBF](documents/1_RBF.md)）
- 线性方程组的求解：自己实现或者学习 Eigen 库的调用（提供[配置教程](documents/eigen_example/README.md)）
- 巩固面向对象编程思想
- 白缝填补（optional）：学习使用 [ANN](documents/ann_example/README.md)


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework2D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework2D` [->](../../Framework2D/) 

作业的基础代码框架

