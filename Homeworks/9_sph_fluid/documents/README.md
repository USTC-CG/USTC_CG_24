# SPH流体仿真简明教程 Part 1. The Way of Water
本次作业我们将进入流体的世界，你将学习使用光滑粒子流体动力学（SPH: Smoothed Particle Hydrodynamics）这种经典的基于粒子表示的仿真方法计算并呈现流体的运动。

> 用>包裹的引用格式呈现的内容为扩展阅读/思考内容，为了实现本次作业可以不看

## 1. 流体如何运动：从Navier-Stokes方程说起 

在游戏与电影中有大量的流体仿真, 下图展示了当前影视界流体仿真的巅峰之作《阿凡达2-水之道》。为了模拟流体多样的运动及其与固体的交互、获得逼真的视觉效果，我们需要从物理出发，考虑最本质的运动方程。

<div  align="center">    
 <img src="../images/avatar2.gif" style="zoom:96%" />
</div>

<div  align="center">    
 <img src="../images/avatar2-3.gif" style="zoom:100%" />
</div>

流体仿真与一般PDE的求解别无二致，都是基于合适的时间离散与空间离散格式，通过数值方法求解描述其运动的PDE。在流体里面我们求解的是Navier-Stokes方程: 

$$
\begin{aligned}
\rho \frac{D \mathbf{v}}{D t} & =\rho g-\nabla p+\mu \nabla^2 \mathbf{v} \tag{1} \\
\nabla \cdot \mathbf{v} & =0 
\end{aligned}
$$

其中 $\rho$ 为流体的密度，$\mathbf{v}$ 为速度场，$g$ 是重力加速度, $p$ 为流体内部压强，$\mu$ 为黏度系数。
第一个方程的右端项分别对应着重力 $\rho g$ 、压力 $-\nabla p$ 、粘性力 $\mu \nabla^2 \mathbf{v}$ 。

> 扩展阅读：我们这里和课上只使用了NS方程的微分形式，但实际上，根据数学建模的不同，NS方程还有其他微分与积分形式。更多内容可以阅读

值得一提的是，(1)式中的 $\frac{D \cdot}{D t}$ 被称为“物质导数 (Material derivative)”：
 
假设我们采样一个以空间位置 $\mathbf{x}$ 和时间 $t$ 为自变量的4维物理场 $f = f(\mathbf{x}, t)$ ，采样点的空间位置 $\mathbf{x} = \mathbf{x}(t)$ 也是时间的函数，那么采样点上的 $f$ 关于 $t$ 的全导数为： $\frac{d f}{d t} = \frac{\partial f}{\partial t} + \frac{\partial f}{\partial x}\frac{\partial x}{\partial t} = \frac{\partial f}{\partial t} + \mathbf{v}\cdot \nabla f$ , 我们将这个全导数称为 $f$ 的物质导数，记为 $\frac{D f}{Dt}$，它同时考虑了 $f$ 由于时间推移和采样点空间移动两个原因带来的变化。

举个例子：坐火车早上从广州出发，晚上到北京，温度场 $T$ 的变化量不仅与时间 $t$ 有关（早上到晚上），也与火车的位置 $\mathbf{x}(t)$ 有关（广州到北京）。

> 和拉格朗日视角 & 欧拉视角的关系？
 
>  $\frac{D \cdot}{D t}$ 还有很多其他名字，如“随体导数 (derivative following the motion)”，这个名字更加直观地反映了其物理含义。[链接](https://www.bilibili.com/video/BV1934y1X7MD/?p=3&share_source=copy_web&vd_source=19d965dd50171e7e3327ff6e149567c2)

NS方程的第一行无法独立求解，为了确定压强 $p$ ， 我们需要考虑流体的不可压缩性条件。

// PPT中的图，散度 和 密度

<div  align="center">    
 <img src="../images/div_v.png" style="zoom:80%" />
</div>

在时间离散上，流体仿真中常用的是一种称为“算子分裂”（Operator Splitting）的做法。

// 根据PPT内容，介绍operator splitting 四步走

1. 不考虑压力更新 $\mathbf{v}$
2. 计算压强
3. 考虑压力更新 $\mathbf{v}$
4. 更新位置


## 2. 如何离散？粒子！

这里我们简要介绍一下SPH的离散方式。都是使用粒子去采样物理场。这些SPH粒子

// 这里需要一个SPH的示意图

<div  align="center">    
 <img src="../images/div_v.png" style="zoom:80%" />
</div>


我们也提供了核函数的代码。

```C++
W()
grad_W()
```

为了查找邻居粒子，这里我们使用了一个空间网格结构。

// 空间加速结构的图

在每一步的开始，我们需要把粒子分配到网格中，然后更新所有粒子的邻居。

```C++
code 
```

下面的代码给出了遍历粒子`p`的所有邻居的方法：

```C++
// Search neighbors
```

为了向大家清楚地展示查找的过程，我们自己编写了邻居粒子查找的代码。如果有兴趣进一步提高程序的性能，你可以尝试使用hw2 image warping中使用过的ANN库。

## 3. 如何运动？

和弹簧质点系统一样，为了让这些离散的粒子动起来，我们需要做一个时间上的离散。 

下图展示了一个流体仿真的通用流程。

// PPT中的通用流程的图。

SPH中密度的计算公式为：

$$
 \rho_i = \sum_j \left(\frac{m_j}{\rho_j} \right) m_j W(\mathbf{x}_i - \mathbf{x}_j, h) =  \sum_j \left(\frac{m_j}{\rho_j} \right) m_j  W_{ij}
$$

这里我们记 $W_{ij} = W(\mathbf{x}_i, \mathbf{x}_j, h)$。 在SPH方法中， $h$ 表示核函数半径，我们用 $\Delta t$ 来表示时间步长。

速度散度的计算公式为：

$$
\nabla \cdot \mathbf{v}_i = \sum_j \frac{m_j}{\rho_j} (\mathbf{v}_j - \mathbf{v}_i) \cdot \nabla W_{ij}
$$

粘性力，我们使用计算公式：

$$
\nabla^2 \mathbf{v}_i=2(d+2) \sum_j \frac{m_j}{\rho_j} \frac{\mathbf{v}_{i j} \cdot \mathbf{x}_{i j}}{\left\|\mathbf{x}_{i j}\right\|^2+0.01 h^2} \nabla W_{i, j}
$$

其中 $d$ 为仿真的维度，这里为3.

> 我们这里没有考虑流体的表面张力

首先，你需要实现第x步，需要填空的代码为：

```
代码片段
```

## 4. 压强是多少？

我们首先介绍一种“弱可压缩的SPH方法” (WCSPH: Weakly Compressible SPH)

所使用的压强计算方式为 

$$
p_i =k_1 \left( \left(\frac{\rho_i}{\rho_0} \right)^{k_2} -1 \right)
$$

压力的加速度： $-\frac{1}{\rho} \nabla p$ ， 其中：

$$
 \nabla p_i = \rho_i \sum_j m_j \left(\frac{p_i}{\rho_i^2} + \frac{p_j}{\rho_j^2} \right) \nabla W_{ij}
$$

> 对称形式与反对称形式，保证动量守恒

那么，本次作业，你需要实现的是：

```
需要填空的代码
```

## 5. 边界处理

边界处理有多种方式。

本次作业我们提供了边界处理的代码。我们采用了简单的反弹策略。


你也可以考虑实现基于ghost particle的做法。

如果实现正确，并且调整了合适的参数，可以看到下面的结果：

// 结果的动图

需要连接的节点图如下:

<div  align="center">    
 <img src="./images/result-with-texture.png" style="zoom:60%" />
</div>


我们提供了一个示例节点图`GeoNodeSystem.json`. 

至此，你已经实现了一个基础的基于SPH的流体仿真系统，但是WCSPH不够稳定，在时间不长调大的时候就会爆炸。

## （Optional）OpenMP 并行

遍历粒子时，可以考虑使用并行。

```C++
```

关于OpenMP的更加详细的介绍可以阅读： // 链接


## 未完待续：Part2. 不可压缩性更好的SPH压力求解器
为了进一步提升仿真的效果和运行效率，我们将在[Part2](./README-part2.md)介绍一种更加稳定的SPH压力计算方法，这也是本次作业的选做内容。

同时，为了让仿真结果更加酷炫，我们将在[Part3](./README-part3.md)介绍从粒子重建表面并进行流体渲染的方法，这也是本次作业的选做内容。

## 参考资料 & 扩展阅读材料
1. [SPH Tutorial]()
2. [游戏中的流体仿真](https://gamerant.com/video-games-best-water-physics/)
3. [太极图形课]()
4. [GAMES103: 基于物理的仿真和动画]()
