import math
from OpenGL.GL import *
from OpenGL.GLUT import *

def myDisplay():
    glClear(GL_COLOR_BUFFER_BIT)

    # 绘制渐变圆
    glBegin(GL_TRIANGLE_FAN)

    # 圆心，颜色为黑色
    glColor3f(0.0, 0.0, 0.0)
    glVertex2f(0.0, 0.0)  # 圆心

    # 绘制圆的边缘，颜色从黑色过渡到红色
    n = 200  # 圆的分段数，越大圆越平滑
    R = 0.5  # 圆的半径
    pi = math.pi
    for i in range(n + 1):
        # 计算每个顶点的角度
        angle = 2 * pi / n * i
        
        # 设置从黑色(0,0,0) 到红色(1,0,0) 的渐变
        red_value = i / n  # 渐变的红色值，i 越大，颜色越红
        glColor3f(red_value, 0.0, 0.0)

        # 绘制圆的顶点
        x = R * math.cos(angle)
        y = R * math.sin(angle)
        glVertex2f(x, y)

    glEnd()
    glFlush()

glutInit()
glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE)
glutInitWindowPosition(100, 100)
glutInitWindowSize(400, 400)
glutCreateWindow("Red Gradient Circle")
glutDisplayFunc(myDisplay)
glutMainLoop()
