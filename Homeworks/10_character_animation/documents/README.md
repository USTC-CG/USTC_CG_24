# 角色动画简明教程

在之前两次作业中，我们都是从物理方程出发求解物质的运动（弹性体、布料、流体），但由于自然界中物理和材质的复杂性，很多生物运动如人体的运动很难通过物理方程的形式准确刻画。本次作业我们将介绍另一种通过预定义骨骼驱动表面mesh运动的方法：骨骼动画，它和动作捕捉等当下流行的电影动画制造方式密切相关。 

> 用>包裹的引用格式呈现的内容为扩展阅读/思考内容，为了实现本次作业可以不看

## 1. 什么是骨骼动画

<div  align="center">    
 <img src="../images/skeleton-illustration.png" style="zoom:40%" />
</div>

骨骼

蒙皮

游戏和动画中十分常用

## 2. 骨骼关节的变换关系

<div  align="center">    
 <img src="../images/skeleton-animation.gif" style="zoom:60%" />
</div>

在上面的动图中，我们可以看到父关节的运动会带动子关节的运动。会形成一个树状结构：

// 此处需要一个树状图

`bindTransform`

那么本次作业中，你需要实现的是[`animator.cpp`](../../../Framework3D/source/nodes/nodes/geometry/character_animation/animator.cpp)中的函数：

```c++
void Joint::compute_world_transform()
{
    // ---------- (HW TODO) Compute world space trasform of this joint -----------------

    // --------------------------------------------------------------------------------
}

void JointTree::compute_world_transforms_for_each_joint()
{
    // ----------- (HW_TODO) Traverse all joint and compute its world space transform ---
	// Call compute_world_transform for each joint
    // ---------------------------------------------
}
```

## 3. 骨骼如何驱动蒙皮运动

蒙皮上的每个顶点可能受到多个关节影响，通过`jointWeight`和`jointIndices`来处理。

你需要实现[`animator.cpp`](../../../Framework3D/source/nodes/nodes/geometry/character_animation/animator.cpp)中的函数：

```c++
void Animator::update_mesh_vertices()
{
	// ----------- (HW_TODO) Update mesh vertices according to the current joint transforms ----
	// 1. get skel_->jointIndices and skel_->jointWeight;
	// 2. For each vertex, compute the new position by transforming the rest position with the joint transforms
	// 2. Update the vertex position in the mesh
	// --------------------------------------------------------------------------------
}
```

> 本次作业使用线性组合。LBS 

## 4. 示例结果 & 节点图

如果关节的transform变换和蒙皮顶点的更新实现正确，从`belly_dance_girl.usda`中可以得到如下的动画结果：

<div  align="center">    
 <img src="../images/hw10-demo.gif" style="zoom:100%" />
</div>

所需要连接的节点图如下：

<div  align="center">    
 <img src="../images/nodes-dance.png" style="zoom:100%" />
</div>

恭喜你，至此已经完成了本次作业的必做部分！

如果有兴趣，可以自行添加新的动画文件进行展示：以下是一些可以考虑的动画资源网站（需要将相应的文件在Blender 4.1中导入并转存为usda格式，才能用于作业框架）：

1. Adobe的免费动画资源网站[Mixamo](https://www.mixamo.com/)，包含角色mesh和多种骨骼动画
2. miHoYo角色MMD资源整理 [链接](https://www.hoyolab.com/article/118389) 

> 拓展阅读：如果考虑一个动作平滑切换到另一个动作，应该怎么办

## (Optional) 骨骼动画 + 布料仿真

本次作业我们还有一个选做部分：通过结合骨骼动画与作业8：弹簧质点仿真实现衣服随人体的运动，将在[Part2：骨骼动画+布料仿真](./README-part2.md)中介绍。

## 参考资料 & 扩展阅读材料