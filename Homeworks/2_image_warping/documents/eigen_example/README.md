# Eigen 示例

## 目录结构
```
eigen_example/
├── CMakeLists.txt          // CMake 配置文件
└── src/                
    ├── _deps/              // Eigen 相关头文件放在这里
    └── eigen_example.cpp   // 测试代码  
```

## 使用说明

我们可以使用 [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) 库来实现线性方程组的求解，这是一个 head-only 的 C++ 库，只需要包含相关的头文件即可使用。

- Step 1: 从[这里](https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip)下载到 Eigen 库的源码，解压，然后将其中 `Eigen/` 文件夹复制到我们目录下的 [_deps/](./src/_deps/) 文件夹中。

- Step 2: 在 CMake 中使用 `target_include_directory` 为项目添加包含目录 `_deps/` （已经写好）。

- Step 3: CMake 配置生成即可，此时在 `eigen_example.cpp` 中可以找到头文件 

```cpp
#include <Eigen/Dense>
```

## 如何在作业项目中使用

将 `Eigen/` 文件夹添加到 `third_party/` 中即可。