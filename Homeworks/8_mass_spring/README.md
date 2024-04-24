# 8. 弹簧质点系统仿真 Mass Spring

<div  align="center">    
 <img src="./images/result-with-texture.png" style="zoom:60%" />
</div>


> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework3D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业


## 作业递交

- 递交内容：程序代码、实验报告及 `Blueprints.json` 文件，见[提交文件格式](#提交文件格式)
- 递交时间：北京时间2024年4月29日（周一）中午12点

## 要求

- 实现基础的半隐式与隐式弹簧质点仿真，主要原理见[文档：弹簧质点系统仿真简明教程 Part 1](documents/README.md)

- （Optional） 实现基于惩罚力的布料与球之间的碰撞处理，主要原理见[文档：弹簧质点系统仿真简明教程 Part 1](documents/README.md)

- （Optional）复现Liu等人2013年的论文[Fast Simulation of Mass-Spring Systems](https://tiantianliu.cn/papers/liu13fast/liu13fast.pdf)中提出的基于Local-Global思想的快速弹簧质点仿真方法，主要原理可以直接阅读论文或[文档: 弹簧质点仿真简明教程 Part 2](documents/README-part2.md) 

## 目的

- 熟悉半隐式与隐式时间积分在基于mesh的仿真中的应用
- 了解弹簧质点系统这一经典的基于物理的仿真方法

## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

作业的基础代码框架

### 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：


### 注意事项

- 导入数据（网格和纹理）时使用相对路径，将你的数据放在可执行文件目录下，直接通过 `FilePath = 'xxx.usda'` 或者 `FilePath = 'zzz.png'` 访问；
- 在 `node_your_implementation.cpp` 等文件中使用
  ```cpp
  #include "utils/some_algorithm.h"
  ```
  包含你的辅助代码；
- 如果除了添加 `utils/` 和 `node_your_implementation.cpp`，你还**对框架有其他修改**，就**打包上传所有代码**。

