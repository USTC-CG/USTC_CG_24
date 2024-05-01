# 9. 基于SPH的流体仿真 SPH Fluid

<div  align="center">    
 <img src="./images/wcsph-demo.gif" style="zoom:100%" />
</div>


> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework3D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业


## 作业递交

- 递交内容：程序代码、实验报告及节点描述文件，见[提交文件格式](#提交文件格式)
- 递交时间：北京时间2024年5月13日（周一）中午12点

## 要求

- 实现弱可压缩的SPH流体仿真方法 WCSPH 的完整流程（包括密度估计、粘性力计算、压力计算、速度与位置更新），主要原理见[文档：SPH流体仿真简明教程 Part 1](documents/README.md)

- （Optional）从粒子重建表面并使用框架中的路径追踪渲染器渲染表面，说明见[文档：SPH流体仿真简明教程 Part 1](documents/README.md)

- （Optional） 实现隐式不可压缩的SPH流体仿真方法 IISPH，主要原理见[文档：SPH流体仿真简明教程 Part 2](documents/README-part2.md)


## 目的

- 了解流体仿真的一般流程
- 了解SPH这一经典的基于粒子表示的仿真方法


## 提供的材料

依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

作业的基础代码框架

### 提交文件格式

文件命名为 `ID_姓名_Homework*.rar/zip`，其中包含：`Framework3D/source/nodes/nodes/geometry/sph_fluid/`文件夹下的所有文件、`Framework3D/source/nodes/nodes/geometry/node_sph_fluid.cpp`, 以及其他可能的自行添加or修改的其他代码文件：
```
ID_姓名_Homework*/
├── sph_fluid/                   
│   ├── xxx.h
│   ├── xxx.cpp
|   └── ...
├── node_sph_fluid.cpp
├── report.pdf                    // 实验报告
├── CompositionGraph.json         // 节点连接信息
├── GeoNodeSystem.json
├── RenderGraph.json
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

