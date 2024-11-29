# 作业一
- [x] 搭建OpenGL编程环境
- [x] 绘制简单的图形 

本次作业基于 Qt 框架完成，使用到了`QOpenGLWidget`

主要的绘制图形代码如下
```C++
void OpenGLWidget::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0); // 设置清除颜色（黑色）
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除颜色和深度缓冲区
    glLoadIdentity(); // 重置当前指定的矩阵为单位矩阵
    // 绘制一个三角形
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0); // 红色
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 1.0, 0.0); // 绿色
    glVertex3f(0.5, -0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0); // 蓝色
    glVertex3f(0.0, 0.5, 0.0);
    glEnd();
}
```
结果如下图所示
![结果](./images/outcome.png "结果")
