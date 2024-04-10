# 6. Shaders

> 作业步骤：
> - 查看[文档](documents/README.md)，内含多个小教程，请先阅读 [documents/README.md](documents/README.md)，其中包含了所有文档的阅读引导
> - 在[项目目录](../../Framework3D/)中编写作业代码
> - 按照[作业规范](../README.md)提交作业

## 作业递交

- 递交内容：程序代码、实验报告及几个节点文件，见[提交文件格式](#提交文件格式)
- 递交时间：2024年4月14日（周日）晚

## 要求

- 实现 Blinn-Phong 着色模型 ([参考资料](https://learnopengl-cn.github.io/02%20Lighting/03%20Materials/))
- 实现 Shadow Mapping 算法 ([参考资料](https://learnopengl-cn.github.io/05%20Advanced%20Lighting/03%20Shadows/01%20Shadow%20Mapping/))
- 可选：实现 Percentage Close Soft Shadow ([参考资料](https://zhuanlan.zhihu.com/p/478472753))
- 可选：实现 Screen Space Ambient Occlusion (参考资料)


## 目的

- 熟悉OpenGL图形API以及GLSL Shader语言
- 了解多Pass绘制方法
- 了解实时渲染中常用的屏幕空间技巧


## 提供的材料

- 基础的光栅化Pass
- 测试场景
依照上述要求和方法，根据说明文档`(1) documents`和作业框架`(2) Framework3D`的内容进行练习。

### (1) 说明文档 `documents` [->](documents/) 

本次作业的要求说明和一些辅助资料

### (2) 作业项目 `Framework3D` [->](../../Framework3D/) 

作业的基础代码框架和测试数据。

测试数据链接：https://rec.ustc.edu.cn/share/e1f568e0-f6e5-11ee-803b-31c46e7802ed

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
├── shaders/                      // 全部shader文件，无论是否修改
|   ├──
│   └── ...  
├── nodes/                        // 你实现or修改的节点文件
│   ├── node_your_implementation.cpp
│   ├── node_your_other_implementation.cpp
│   └── ...  
├── CompositionGraph.json               // 节点连接信息
├── GeoNodeSystem.json
├── RenderGraph.json
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