#include "openglwidget.h"
#include <QOpenGLFunctions>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{

}

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

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h); // 设置视口大小
}




