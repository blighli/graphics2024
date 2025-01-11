#include <GL/glut.h>

// 绘制三角形的回调函数
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // 清空颜色缓冲

    // 设置绘图颜色 (红色)
    glColor3f(1.0f, 0.0f, 0.0f);

    // 开始绘制三角形
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f); // 左下角
    glVertex2f(0.5f, -0.5f);  // 右下角
    glVertex2f(0.0f, 0.5f);   // 顶点
    glEnd();

    glFlush(); // 强制刷新缓冲区，显示内容
}

// 初始化 OpenGL 设置
void init() {
    // 设置背景颜色为深蓝色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

// 主函数
int main(int argc, char** argv) {
    glutInit(&argc, argv);                         // 初始化 GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);   // 设置显示模式
    glutInitWindowSize(800, 600);                  // 设置窗口大小
    glutCreateWindow("Triangle "); // 创建窗口

    init();                                        // 调用初始化函数
    glutDisplayFunc(display);                      // 设置绘制回调函数

    glutMainLoop();                                // 进入 GLUT 事件处理循环
    return 0;
}
