# ANN 示例

## 目录结构
```
ann_example/
├── CMakeLists.txt      // CMake 配置文件
└── src/                
    ├── _deps/          // ANN 相关头文件放在这里
    └── ann_example.cpp // 测试代码  
```

## 使用说明

我们可以使用 [annoy](https://github.com/spotify/annoy)(Approximate Nearest Neighbors Oh Yeah) 库来实现最近邻的查找，这是一个 head-only 的 C++ 库，只需要包含相关的头文件即可使用。

- Step 1: 从[这里](https://github.com/spotify/annoy/releases/tag/v1.17.2)下载到 annoy 库的源码，解压，然后将其中 `src/` 文件夹下的 `annoylib.h`，`kissrandom.h`，`mman.h` 文件复制到我们目录下的 [_deps/](./src/_deps/) 文件夹中。

- Step 2: 在 CMake 中使用 `target_include_directory` 为项目添加包含目录 `_deps/` （已经写好）。

- Step 3: CMake 配置生成即可，此时在 `ann_example.cpp` 中可以找到头文件 

```cpp
#include <annoylib.h>
```


## 如何在作业项目中使用

将相关头文件添加到 `third_party/` 中即可。