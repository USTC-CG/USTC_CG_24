# Q&A

## 如何使用Release模式？

因为我们并不能假设所有同学都有支持光线追踪的显卡，本次作业使用CPU对光照进行计算。此过程的计算量很大，因此我们需要开启release模式。

在release模式下，我们没有办法进行断点调试，但是这时编译器会对我们的代码进行优化，改进其生成二进制代码的效率。

在visual studio中，我们这样开启（初始状态下，大家可能没有x64-Release）：
![alt text](image-3.png)

在`管理配置`中，点击+号，并将配置类型更改为release（默认状态会是RelWithDebInfo）
![alt text](image-4.png)

使用VSCode的同学，
![alt text](image-6.png)

且需要勾选
![alt text](image-5.png)