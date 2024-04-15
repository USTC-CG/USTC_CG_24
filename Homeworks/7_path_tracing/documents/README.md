# Document

## 作业步骤

### 实现直接光照积分器

初始状态下，我们能得到当前结果：
![alt text](image.png)

事实上，我们已经完成了直接光照积分器的实现。本部分作业需要额外对一种光源实现支持，即`Hd_USTC_CG_Rect_Light`。它位于Light.h/Light.cpp的最末尾，同学们需要参考同文件中的Sphere Light完成其中的几个函数。


### 路径追踪算法

![alt text](image-1.png)

我们可以在这里，将其中的`DirectLightIntegrator`切换为`PathIntegrator`来开始下一步的实现。

  - Russian Roullete
- (Optional) 添加一种材质，对材质进行重要性采样，并进行多重重要性采样，与单种采样的结果进行比较
- (Optional) 透明材质