# MiniDraw Step-by-Step

## 目标和要求

- 在 [Canvas 类](../../../Framework2D/include/view/comp_canvas.h) 中添加绘制新图形 `Ellipse`, `Polygon`, `Freehand` 的功能；
- 在 [MiniDraw 窗口类](../../../Framework2D/src/assignments/1_MiniDraw/window_minidraw.cpp) 中添加相应的交互按钮；
- 学习类的**继承**和**多态**。
  
## 说明

- 请确保已经按照 [ImGui 程序框架配置说明](framework_introduction.md) 配置成功框架代码，这个时候应该可以运行成功 `1_MiniDraw` 项目，已经实现了 `Line` 和 `Rectangle` 的绘制；
- 善用 VS Code 的全局搜索功能，快捷键 `Ctrl+Shift+F`，例如你可以使用这个功能全局搜索 `HW1_TODO` 的提示，帮助快速定位到关键部分。
- **符合项目要求的结构设计、实现方法有很多，你不一定要严格按照下面的提示来实现，如果你有更好的想法，请务必实现它，并且在报告文件中详细描述。**

## 1. 添加椭圆绘制功能

在作业代码里面已经添加好了直线和矩形线框的绘制功能，可以简单参考 [ImGui 程序框架具体实现](framework_details.md)，添加椭圆的过程是类似的。

### Step 1：在主窗口中添加一个 `Ellipse` 按钮交互

在 [window_minidraw.cpp](../../../Framework2D/src/assignments/1_MiniDraw/window_minidraw.cpp) 的 `draw_canvas()` 中添加一个按钮，实现椭圆图形类型的切换，可以仿照前面直线段和矩形的按钮设计。

单击这个按钮的时候， `p_canvas` 需要执行 `set_ellipse()` 操作切换自己的图形类型。你需要先在 [comp_canvas.h](../../../Framework2D/include/view/comp_canvas.h) 为 `Canvas` 类添加一个 `set_ellipse()` 方法，并在 [comp_canvas.cpp](../../../Framework2D/src/view/comp_canvas.cpp) 中实现它，可以参考`set_line()`、`set_rect()`的实现。这样就可以正确切换图形类型了。

不过这个时候我们还没有实现椭圆对应的数据结构和绘制方法，因此画不出椭圆。

### Step 2: 实现 ``Ellipse`` 图形类

在 [include/view/shapes/](../../../Framework2D/include/view/shapes/) 文件夹下新建一个 `ellipse.h` 文件，在 [src/view/shapes/](../../../Framework2D/src/view/shapes/) 文件夹下新建一个 `ellipse.cpp` 文件。他们将用来实现椭圆类 `Ellipse`。为了让项目读取到文件结构的更新，可以在 VS Code 中重新执行一次 CMake 配置。

椭圆类的实现请参考同文件夹下直线类 `Line`、`Rect` 类，你需要为椭圆定义一个数据存储的结构，一个构造方法，并实现一个椭圆的绘制函数 `draw()`，这里 ImGui 为我们提供了一个现有的方法用以绘制椭圆：

```cpp
void AddEllipse(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot = 0.0f, int num_segments = 0, float thickness = 1.0f);
```

为了实现椭圆形状的动态更新，可以仿照 `Line` 和 `Rect` 类写一个 `update(float x, float y)` 函数，它用传入的二维鼠标位置更新椭圆内存储的数据。

> **思考：`Class Ellipse` 是如何体现类的继承的。**

### Step 3: 在 `comp_canvas.cpp` 中实现鼠标绘制椭圆

现在我们已经实现了椭圆类，并且为 `Canvas` 添加了椭圆类型的绘制状态，下一步将在椭圆状态 `shape_type_ == kEllipse` 下实现鼠标绘制椭圆。实现了 `comp_canvas.cpp` 中的一处鼠标事件的 `HW1_TODO` 后，可以做到效果： “鼠标单击开始绘制椭圆，移动鼠标自动更新椭圆，再次单击鼠标结束绘制”。

- 在 `comp_canvas.cpp` 中包含椭圆头文件 `ellipse.h`；
- 在函数 `mouse_click_event()` 中用构造方法创建一个椭圆，让 `current_shape_` 指向它；

剩下的操作都是已经完成了的，不需要额外补充：
- 函数 `mouse_move_event()` 中调用 `update` 函数动态更新 `current_shape_`；
- 函数 `mouse_click_event()` 在绘画状态中单击时退出绘画状态，并存储当前图形。

至此，椭圆的绘制程序就完成了。

> **思考：`Class Ellipse` 是如何体现类的多态的。**

## 2. 添加多边形绘制功能

添加多边形绘制功能和前三个图形是类似的，不过交互上会稍有不同，同学们可以自己思考实现的方法，下面给出一种简单实现方案：

### Step 1: 在主窗口中添加一个 `Polygon` 按钮交互

### Step 2: 实现多边形图形类 `Class Polygon`

> **思考：多边形的数据应该如何存储？**

对于多边形，实现 `update(float x, float y)` 函数可以用于为其添加顶点，绘制函数 `draw()` 可以将其分解为多段直线绘制。

### Step 3: 在 `comp_canvas.cpp` 中实现鼠标绘制多边形

可以按照这样的逻辑：鼠标左键单击时，创建一个多边形对象，后续鼠标左键单击的时候为多边形添加顶点，直到鼠标右键单击结束多边形的创建。

相应地要修改 `mouse_click_event()`, `mouse_move_event()`, `mouse_release_event()` 中的中多边形相关的逻辑。

> **Freehand 图形的绘制事实上和多边形较为类似，实现思路是一致的。**




