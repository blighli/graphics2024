'''
	@author ： yanan-zhang
	@software : PyCharm
	@file : tri.py
	@time : 2025/1/4 16:15
'''
import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import numpy as np

# 顶点着色器代码
vertex_shader = """
# version 330 core
layout(location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
"""

# 片段着色器代码
fragment_shader = """
# version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // 红色
}
"""

# 初始化GLFW
if not glfw.init():
    raise Exception("GLFW 初始化失败")

# 创建窗口
window = glfw.create_window(800, 600, "OpenGL 基础图形", None, None)
if not window:
    glfw.terminate()
    raise Exception("创建窗口失败")

glfw.make_context_current(window)

# 编译着色器程序
shader = compileProgram(
    compileShader(vertex_shader, GL_VERTEX_SHADER),
    compileShader(fragment_shader, GL_FRAGMENT_SHADER)
)

# 定义三角形的顶点
vertices = np.array([
    0.0,  0.5, 0.0,  # 顶点1
   -0.5, -0.5, 0.0,  # 顶点2
    0.5, -0.5, 0.0   # 顶点3
], dtype=np.float32)

# 创建VBO和VAO
VAO = glGenVertexArrays(1)
VBO = glGenBuffers(1)

glBindVertexArray(VAO)

glBindBuffer(GL_ARRAY_BUFFER, VBO)
glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * vertices.itemsize, None)
glEnableVertexAttribArray(0)

glBindBuffer(GL_ARRAY_BUFFER, 0)
glBindVertexArray(0)

# 渲染循环
while not glfw.window_should_close(window):
    # 处理输入
    glfw.poll_events()

    # 清屏
    glClearColor(0.2, 0.3, 0.3, 1.0)
    glClear(GL_COLOR_BUFFER_BIT)

    # 使用着色器程序
    glUseProgram(shader)

    # 绘制三角形
    glBindVertexArray(VAO)
    glDrawArrays(GL_TRIANGLES, 0, 3)
    glBindVertexArray(0)

    # 交换缓冲区
    glfw.swap_buffers(window)

# 清理资源
glDeleteVertexArrays(1, [VAO])
glDeleteBuffers(1, [VBO])

# 终止GLFW
glfw.terminate()
