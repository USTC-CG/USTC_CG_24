# 5. ARAP参数化 ARAP Parameterzation

> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework2D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码、实验报告及 `Blueprints.json` 文件，见[提交文件格式](#提交文件格式)
- 递交时间：2024年4月2日（周二）晚

## 要求

- 实现论文 [A Local/Global Approach to Mesh Parameterization](https://cs.harvard.edu/~sjg/papers/arap.pdf) 中介绍的 ARAP (As-rigid-as-possible) 网格参数化方法，主要原理见[作业课件](https://rec.ustc.edu.cn/share/c55d42a0-bfcd-11ee-b7db-eb3ed86abde8)

- （Optional）实现论文中的另外两种参数化
  - ASAP（As-similar-as-possible）参数化算法
  - Hybrid 参数化算法
- 使用测试纹理和网格检验实验结果


## 目的

- 对各种参数化算法（作业4、作业5）进行比较
- 了解非线性优化
- 继续学习网格数据结构和编程
- 巩固大型稀疏线性方程组的求解方法
- 理解并实现（二阶）矩阵的 SVD 分解


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework2D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework2D` [->](../../Framework2D/) 

作业的基础代码框架

## 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：

```
ID_姓名_Homework*/
├── data/                         // 测试模型和纹理
│   ├── xxx.usda
│   ├── yyy.usda
│   ├── zzz.png
│   └── ...  
├── utils/                        // 辅助代码文件
│   ├── some_algorithm.h
│   ├── some_algorithm.cpp
│   └── ...  
├── nodes/                        // 你实现or修改的节点文件
│   ├── node_your_implementation.cpp
│   ├── node_your_other_implementation.cpp
│   └── ...  
├── Blueprints.json               // 节点连接信息
├── report.pdf                    // 实验报告
└── ...                           // 其他补充文件

```

### 注意事项

- 导入数据（网格和纹理）时使用相对路径，将你的数据放在可执行文件目录下，直接通过 `FilePath = 'xxx.usda'` 或者 `FilePath = 'zzz.png'` 访问；
- 在 `node_your_implementation.cpp` 等文件中使用
  ```cpp
  #include "utils/some_algorithm.h"
  ```
  包含你的辅助代码；
- 如果除了添加 `utils/` 和 `node_your_implementation.cpp`，你还**对框架有其他修改**，就**打包上传所有代码**。