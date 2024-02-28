# 1. 画图小工具 MiniDraw

> 作业步骤：
> - 查看[文档](documents/)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework2D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告 
- 递交时间：2024年3月3日（周日）晚

## 要求

- 写一个交互式画图小程序 MiniDraw，要求画直线 (Line)，椭圆 (Ellipse)，矩形 (Rectangle)，多边形 (Polygon) 等图形元素（图元）
- 每种图元需用一个类（对象）来**封装**，如 `Line`，`Ellipse`，`Rect`，`Polygon`，`Freehand`（自由绘制模式，optional）
- 各种图元从一个父类来**继承**，如 `Shape` 
- 使用类的**多态**调用绘图函数
- 画图工具的拓展功能（线条粗细颜色、形状填充、对象选取、对象变换、顶点编辑等，optional）
- 注意代码性能、规范性等
- 提供规范、详细的实验报告

## 目的

- 学习编写图形用户界面（GUI）
- STL `vector` 等的使用
- 学习面向对象编程
  - 巩固类的封装性
  - 学习体会类的继承和多态
- 多使用 Online Help

## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework2D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework2D` [->](../../Framework2D/) 

作业的基础代码框架

