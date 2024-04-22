# 弹簧质点系统仿真简明教程 Part 2 之加速方法

如果你已经完成了Part 1，那么欢迎你来到弹簧质点系统仿真的进阶教程。我们将介绍刘天添老师在论文[Fast Simulation of Mass-Spring Systems](https://tiantianliu.cn/papers/liu13fast/liu13fast.pdf)中提出的加速方法，并解释论文中公式的含义。

> 用>包裹的引用格式为扩展阅读内容。为了实现作业可以不看

## 1. 弹性能量的新视角

在Part 1中我们说过，弹簧质点系统中每一根弹簧的能量可以定义为（Liu的论文中使用$\mathbf{p}$表示顶点位置）:

$$
E_i = \frac{1}{2} k (\|\mathbf{x}_{i}\| -L)^2  \tag{1}
$$

其中$\mathbf{x}_i = \mathbf{x}_{i1} - \mathbf{x}_{i2}$。总的弹性能为$E = \sum_i E_i$，这是一个非凸的能量。

对于(1)式中的能量，我们之前看待的角度为：给定原长为$L_i$的弹簧，让$\mathbf{x}_i$变化，不同的$\mathbf{x}_i$带来了不同的能量。$\mathbf{x}_i$具有旋转的自由度，只要其范数$= L_i$即可最小化能量。

但是Liu等人提出，我们可以从另一个角度看待这个能量：如果固定$\mathbf{x}_i$，让弹簧原始的长度与方向变化，也就是把弹簧表示为一个向量$\mathbf{d}$，那么 $(\|\mathbf{x}_{i}\| - L)^2$ 可以被看作是一个优化问题的解： $\min_{\| \mathbf{d}\| =r}\| \mathbf{x}_{i} - \mathbf{d}  \|^2$。 通过“三角形两边之差一定小于第三边”的原理我们可以容易地证明这一点，如下图所示。

<div  align="center">    
 <img src="../images/liu13-1.png" style="zoom:40%" />
</div>

那么， 通过将单根弹簧的能量写为包含$\mathbf{x}_i$与$\mathbf{d}_i$两个变量的函数 $ E_i = \frac{1}{2} k \|\mathbf{x}_{i} -\mathbf{d}_i\|^2 $, 总能量可以写为：

$$
E = \sum_i E_i = \frac{1}{2} \sum_i k_i \|\mathbf{x}_i - \mathbf{d}_i \|^2 \tag{2}
$$

将二范数展开：$\|\mathbf{x}_i - \mathbf{d}_i \|^2  = (\mathbf{x}_i - \mathbf{d}_i)^\top (\mathbf{x}_i - \mathbf{d}_i )$, 可以将公式(2)整理为矩阵相乘的形式：

$$
E = \frac{1}{2} \mathbf{x}^\top \mathbf{L}\mathbf{x} - \mathbf{x}^\top \mathbf{J} \mathbf{d}  \tag{3}
$$

其中$\mathbf{L} \in \R^{3n\times 3n}, \mathbf{J} \in \R^{3n \times 3s}$, $n$为顶点个数，$s$为弹簧个数（边的个数）：
$$
\mathbf{L}=\left(\sum_{i=1}^s k_i \mathbf{A}_i \mathbf{A}_i^{\top}\right) \otimes \mathbf{I}_3, \\
\mathbf{J}=\left(\sum_{i=1}^s k_i \mathbf{A}_i \mathbf{S}_i^{\top}\right) \otimes \mathbf{I}_3
$$

其中 $\mathbf{A}_i \in \R^{n}$为" the incidence vector of i-th spring"，$\mathbf{S}_i \in \R^{s}$为"the
i-th spring indicator"，它们的构建如下所示：

<div  align="center">    
 <img src="../images/A_illustration.png" style="zoom:60%" />
</div>


<div  align="center">    
 <img src="../images/S_illustration.png" style="zoom:60%" />
</div>

Ok, 现在通过引入新的变量$\mathbf{d}$，我们将之前(1)式中简单的弹性能变成了(3)这种看似更加复杂形式，这样子到底有什么好处呢？下一节就会很快看到。

## 2. 双向奔赴的Local-Global Solver

上一节我们给出了新视角下的弹簧能量，结合Part 1中对隐式欧拉积分能量的介绍，

> Reminder：每一步优化的能量为：
> $$ \min_{\mathbf{x}} \quad g(\mathbf{x}) = \frac{1}{2 h^2}(\mathbf{x} - \mathbf{y})^\top   \mathbf{M} (\mathbf{x} - \mathbf{y}) + E(\mathbf{x}) 
> $$， 
> 其中$E$为弹性能。

那么，我们需要优化的总能量为：

$$
g(\mathbf{x})=\min_{\mathbf{x}, \mathbf{d} \in U}  \frac{1}{2 h^2}(\mathbf{x} - \mathbf{y})^\top   \mathbf{M} (\mathbf{x} - \mathbf{y}) + \frac{1}{2} \mathbf{x}^{\boldsymbol{\top}} \mathbf{L} \mathbf{x}-\mathbf{x}^{\top} \mathbf{J} \mathbf{d} \\

= \min_{\mathbf{x}, \mathbf{d} \in U} \frac{1}{2 h^2} \mathbf{x}^\top(\mathbf{M} + h^2 \mathbf{L})\mathbf{x} - \mathbf{x}^\top\mathbf{J}\mathbf{d} + \mathbf{x}^\top \mathbf{b} + C
$$

$U$为$\mathbf{d}$组成的集合。

之前是单向调整$\mathbf{x}$，现在是$\mathbf{x}$和$\mathbf{d}$两个方向双向奔赴。和ARAP原理类似。
$L$只需要算一次，可以预分解

> Local-Global可以看做是一种交替方向迭代。

那么在实际中，我们就先固定$\mathbf{x}_i$，对每根弹簧求解$\mathbf{d}_i$（Local Step），这个操作可以并行：

$$ \mathbf{d}_i = L_i \frac{\mathbf{x}_i}{\|\mathbf{x}_i \|} $$

然后再固定$\mathbf{d}_i$, 求解$\mathbf{x}_i$，这一过程会转化为下面这个线性方程组的求解（Global Step）：

$$
\mathbf{A} \mathbf{x} = \mathbf{b}
$$

其中$\mathbf{A} = \mathbf{M} + h^2 \mathbf{L} \in \R^{3n \times 3n}$ 以及 $\mathbf{b} = h^2 \mathbf{Jd} \in \R^{3n}$, $\mathbf{x} \in \R^{3n}$. 

并且$\mathbf{A}$为正定的，所以我们可以对$\mathbf{A}$进行预分解，这样子只需要在初始化的时候计算即可。

在本次作业中，你需要完成的是`FastMassSpring`类中构造函数中的$\mathbf{A}$的计算与预分解：

```C++ 
FastMassSpring::FastMassSpring(const Eigen::MatrixXd& X, const EdgeSet& E) : MassSpring(X, E)
{
    unsigned n_vertices = X.rows();
    Eigen::SparseMatrix<double> A(n_vertices * 3, n_vertices * 3);
    A.setZero();
    // ... 
    // compute A here

    A.makeCompressed();
    // Prefactorize A here 
}
```

和`step`函数: 

```C++
void FastMassSpring::step()
{
    // for (unsigned iter = 0; iter < max_iter; iter++) {
        
        // Local-Global alternating solving
    
    // }
}
```

在选择合适的劲度系数、时间步和迭代次数后，可以看到类似下图所示的结果：

<div  align="center">    
 <img src="../images/fast_mass_spring.gif" style="zoom:100%" />
</div>

此次作业所需要连接的节点图与Part 1一样，只需要通过打开开关`enable Liu13`来切换到该加速方法。


值得一提的是，Liu指出，这一加速方法在迭代次数增加时的结果会逐渐接近Newton法求解的结果，如下图所示。**鼓励大家在报告中包含不同迭代次数的仿真结果的比较**。

<div  align="center">    
 <img src="../images/liu-iterations.png" style="zoom:80%" />
</div>

> 扩展阅读：最后值得一提的是，该加速方法前期收敛速度快，但是后期残差下降慢。为什么？
> <div  align="center">    
> <img src="../images/liu13-results.png" style="zoom:80%" />
> </div>


## 3. 总结

投影动力学方法（Projective Dynamics）的前身。更多的能量模型和离散方式。更多的问题。

> 拓展思考：更多的加速方法：拟牛顿法及其变种（图形学中常用的是L-BFGS）

### 参考资料
1. 论文[Fast Simulation of Mass-Spring Systems](https://tiantianliu.cn/papers/liu13fast/liu13fast.pdf)
2. [SIGGRAPH Asia 2013 Talk](https://www.youtube.com/watch?v=vmdBHde8BL8)