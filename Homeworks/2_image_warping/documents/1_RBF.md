# Radial basis functions interpolation method [^RBF] 

## 问题描述

给定 $n$ 对控制点 $(\boldsymbol{p}_i, \boldsymbol{q}_i)$ ，其中 $\boldsymbol{p}_i,\boldsymbol{q}_i\in\mathbb{R}^2$ ， $i=1, 2, \cdots,n$ ，

希望得到一个函数 $f : \mathbb{R}^2\to\mathbb{R}^2$ ，满足插值条件：

$$f(\boldsymbol{p} _ i) = \boldsymbol{q} _ i, \quad \text{for } i = 1, 2, \cdots, n.$$

## 算法原理

假设所求的插值函数 $f$ 是如下径向基函数组合的形式

$$f(\boldsymbol{p})=\sum _ {i=1}^n \boldsymbol{\alpha} _ i R(\Vert\boldsymbol{p}-\boldsymbol{p} _ i\Vert)+\boldsymbol{A}\boldsymbol{p}+\boldsymbol{b},$$

其中，

- $R(\Vert\boldsymbol{p}-\boldsymbol{p}_ i\Vert)$ 是 $n$ 个径向基函数，例如可选 $R(d)=(d^2+r^2)^{\mu/2}$ ，其系数 $\boldsymbol{\alpha}_i\in\mathbb{R}^2$ 待定；
- $\boldsymbol{A}\in\mathbb{R}^{2\times 2}$ 和 $\mathbf{b}\in\mathbb{R}^2$ 是待定的仿射部分. 

该映射有 $2(n+3)$ 个自由度，插值条件

$$f(\boldsymbol{p} _ j)=\sum _ {i=1}^n\boldsymbol{\alpha} _ i R(\Vert\boldsymbol{p} _ j-\boldsymbol{p} _ i\Vert)+A\boldsymbol{p} _ j+\boldsymbol{b}=\boldsymbol{q} _ j,\quad j=1,\dots,n.$$

提供了 $2n$ 个约束，可选的补充约束为

$$\begin{pmatrix}
\boldsymbol{p} _ 1 & \cdots &\boldsymbol{p} _ n \newline
1 & \cdots &1
\end{pmatrix} _ {3\times n}
\begin{pmatrix}
\boldsymbol{\alpha} _ 1^\top\newline
\vdots\newline
\boldsymbol{\alpha} _ n^\top
\end{pmatrix} _ {n\times 2} = \boldsymbol{0} _ {3\times 2}.$$

也可根据论文，通过额外的仿射集中的控制点 $(\boldsymbol{x} _ i,\boldsymbol{y} _ i)$ 来确定 $\boldsymbol{A}$ 和 $\boldsymbol{b}$ 

- 没有点时，恒等变换（ $\boldsymbol{A}=\boldsymbol{I},\boldsymbol{b}=\boldsymbol{0}$ ）
- 一个点时，平移变换（ $\boldsymbol{A}=\boldsymbol{I}, \boldsymbol{b}=\boldsymbol{y}_i-\boldsymbol{x}_i$ ）
- 两个点时，平移+缩放
- 三个点时，一般仿射变换
- 多个点时，用最小二乘法求仿射变换

## 参考文献

[^RBF]: Arad N, Reisfeld D. [**Image warping using few anchor points and radial functions**](https://onlinelibrary.wiley.com/doi/10.1111/1467-8659.1410035 )[C]//Computer graphics forum. Edinburgh, UK: Blackwell Science Ltd, 1995, 14(1): 35-46.

