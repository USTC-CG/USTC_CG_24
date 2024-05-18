# Part 2: 骨骼动画 + 布料仿真

在这一部分，我们将尝试结合HW10和HW8，实现一个如下的裙子随着人体一起运动的结果

<div  align="center">    
 <img src="../images/girl-with-cloth-intersect.gif" style="zoom:70%" />
</div>

## 1. 布料与物体的连接

<div  align="center">    
 <img src="../images/arm-with-cloth.gif" style="zoom:70%" />
</div>

在Blender中编辑顶点组。

`init_dirichlet_bc_vertices_control_pair`


## 2. 布料自碰撞 & 布料与人体碰撞的处理 

布料自碰撞可以参考HW8的优秀作业分享。




## 3. 提供的初始结果

下图的结果没有考虑布料自碰撞和与人体碰撞

<div  align="center">    
 <img src="../images/girl-with-cloth-intersect.gif" style="zoom:70%" />
</div>

需要连接的节点图为（已经提供在了[`../data/GeoNodeSystem-girl-with-skirt.json`](../data/GeoNodeSystem-girl-with-skirt.json)）：

<div  align="center">    
 <img src="../images/node2.png" style="zoom:70%" />
</div>

可以自行实现第2节中介绍的布料自碰撞处理方法 & 布料与人体碰撞的处理方法。并在报告中介绍你采取的方法以及处理前后的结果对比动图/视频。

## 参考资料
[GAMES103]