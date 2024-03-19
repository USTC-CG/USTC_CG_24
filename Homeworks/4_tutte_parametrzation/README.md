# 3. 曲面参数化 Tutte Parameterzation

> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework2D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码及实验报告 
- 递交时间：2024年3月24日（周日）晚

## 要求

- 实现论文 [Floater1997](https://www.cs.jhu.edu/~misha/Fall09/Floater97.pdf) 中介绍的 Tutte 网格参数化方法，主要原理见[作业课件](https://rec.ustc.edu.cn/share/c55d42a0-bfcd-11ee-b7db-eb3ed86abde8)
  - 边界固定，构建并求解稀疏方程组得到**极小曲面**
  - 边界映射到平面凸多边形，求解稀疏方程组得到**曲面参数化**
- 尝试多种（2~3种）权重设置
  - Uniform weights
  - Cotangent weights
  - Floater weights (Shape-preserving)（Optional）
- 使用测试纹理和网格检验实验结果


## 目的

- 熟悉网格数据结构，了解基本操作方法（访问邻域、访问边界）（参考示例）
- 了解纹理映射，对参数化结果进行可视化
- 了解节点编程思想
- 巩固使用大型稀疏线性方程组的求解


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework2D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework2D` [->](../../Framework2D/) 

作业的基础代码框架
