# 3. 图像融合 Poisson Image Editing

> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework2D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告 
- 递交时间：2024年3月17日（周日）晚

## 要求

- 实现 Poisson Image Editing 算法（矩形边界）
  - 至少须实现文中的 Seamless cloning 的应用
- 实时拖动区域显示结果
- 复杂边界实现（多边形光栅化的[扫描线转换算法](documents/ScanningLine.md)）（Optional）

## 目的

- 学习使用 Eigen 求解大型稀疏方程组
- 使用矩阵预分解提高计算效率
- 了解多边形的扫描线算法


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework2D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework2D` [->](../../Framework2D/) 

作业的基础代码框架
