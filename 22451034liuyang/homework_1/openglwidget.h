#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
protected:
    void initializeGL() override; // 初始化OpenGL
    void paintGL() override;     // OpenGL绘制
    void resizeGL(int w, int h) override; // 调整视口大小
};

#endif // OPENGLWIDGET_H
