# Inverse distance-weighted interpolation methods[^IDW] 

## 问题描述

给定 $n$ 对控制点 $(\boldsymbol{p}_i, \boldsymbol{q}_i)$ ，其中 $\boldsymbol{p}_i,\boldsymbol{q}_i\in\mathbb{R}^2$ ， $i=1, 2, \cdots,n$ ，

希望得到一个函数 $f : \mathbb{R}^2\to\mathbb{R}^2$ ，满足插值条件：

$$
f(\boldsymbol{p}_i) = \boldsymbol{q}_i, \quad \text{for } i = 1, 2, \cdots, n.
$$

## 算法原理

假设所求的插值函数 $f$ 具有如下加权平均的形式

$$
f(\boldsymbol{p})=\sum_{i=1}^n w_i(\boldsymbol{p})f_i(\boldsymbol{p}),
$$

其中每个 $f_i$ 是点 $\boldsymbol{p}_i$ 处的局部近似（local approximation），满足在 $(\boldsymbol{p}_i, \boldsymbol{q}_i)$ 处的插值性质；权重函数 $w_i$ 满足非负性和归一性

$$
\sum_{i=1}^n w_i = 1, \text{and } \ w_i \geq 0, \text{ for } i = 1, 2, \cdots, n,
$$

且在 $\boldsymbol{p}_i$ 处 $w_i(\boldsymbol{p}_i) = 1$.

对于任意的 $i$，

- 权重 $w_i: \mathbb{R}^2\to\mathbb{R}$ 形如

  $$w_i(\boldsymbol{p}) = \dfrac{\sigma_i(\boldsymbol{p})}{\displaystyle\sum_{j=1}^n\sigma_j(\boldsymbol{p})},$$

  可选 $\sigma_i(\boldsymbol{p}) = \dfrac{1}{\Vert\boldsymbol{p} - \boldsymbol{p}_i\Vert^\mu}$ ， $\mu > 1$. 

- 映射 $f_i: \mathbb{R}^2\to\mathbb{R}^2$ 形如 
  
  $$f_i(\boldsymbol{p})=\boldsymbol{q}_i+\boldsymbol{D}_i(\boldsymbol{p}-\boldsymbol{p}_i),$$
  
  其中 $\boldsymbol{D} _ i:\mathbb{R}^2\to\mathbb{R}^2$ ，满足 $\boldsymbol{D} _ i(\boldsymbol{0})=\boldsymbol{0}$ . 可选 $\boldsymbol{D} _ i$ 为线性变换，即 $\boldsymbol{D}_i \in \mathbb{R}^{2\times 2}$ .

简单地，可直接取 $\boldsymbol{D}_i=\boldsymbol{0}$，此时

$$
f(\boldsymbol{p})=\sum_{i=1}^n w_i(\boldsymbol{p})\boldsymbol{q}_i.
$$

也可以对每个 $i$ 求更精确的 $\boldsymbol{D}_i$，通过最小化下面的能量

$$
E_i(\boldsymbol{D} _ i) = \sum _ {j=1, j\neq i}^n \sigma_i(\boldsymbol{p}_j)\left\Vert\boldsymbol{q}_i+\boldsymbol{D}_i(\boldsymbol{p}_j-\boldsymbol{p}_i) - \boldsymbol{q}_j\right\Vert^2.
$$

## 参考文献

[^IDW]: Ruprecht D, Muller H. [**Image warping with scattered data interpolation**](https://ieeexplore.ieee.org/document/365004)[J]. IEEE Computer Graphics and Applications, 1995, 15(2): 37-43.

