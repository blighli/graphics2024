import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
from PIL import Image
import numpy as np

# 初始化窗口大小
WINDOW_WIDTH = 800
WINDOW_HEIGHT = 600

# 光源位置
light_position = [1.2, 1.0, 2.0, 1.0]  # 光源位置

def load_texture(file_path):
    """
    加载纹理图片
    """
    texture = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)

    # 加载图片
    image = Image.open(file_path)
    image = image.transpose(Image.FLIP_TOP_BOTTOM)  # 翻转图片以适应OpenGL纹理坐标
    img_data = np.array(list(image.getdata()), np.uint8)

    # 生成纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data)
    glBindTexture(GL_TEXTURE_2D, 0)  # Unbind texture after loading
    return texture

def setup_lighting():
    """
    配置光照
    """
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightfv(GL_LIGHT0, GL_POSITION, light_position)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    glLightfv(GL_LIGHT0, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])

    # 也为材质设置一些属性，使其能够反射光线
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    glMaterialfv(GL_FRONT, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0)

def draw_sphere(radius, texture_id):
    """
    绘制球体并应用纹理
    """
    glEnable(GL_TEXTURE_2D)
    glBindTexture(GL_TEXTURE_2D, texture_id)
    gluQuadric = gluNewQuadric()
    gluQuadricTexture(gluQuadric, GL_TRUE)
    gluSphere(gluQuadric, radius, 32, 32)
    gluDeleteQuadric(gluQuadric)
    glBindTexture(GL_TEXTURE_2D, 0)  # Unbind texture after using

def main():
    if not glfw.init():
        return

    window = glfw.create_window(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Solar System", None, None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)
    glEnable(GL_DEPTH_TEST)

    # 加载纹理
    earth_texture = load_texture("textures/earth.jpg")
    sun_texture = load_texture("textures/sun.jpg")

    # 设置光照
    setup_lighting()

    # 设置投影矩阵
    glMatrixMode(GL_PROJECTION)
    gluPerspective(45, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 50.0)
    glMatrixMode(GL_MODELVIEW)

    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0)

        # 绘制太阳
        glPushMatrix()
        glTranslatef(-1.5, 0, 0)
        draw_sphere(0.8, sun_texture)
        glPopMatrix()

        # 绘制地球
        glPushMatrix()
        glTranslatef(1.5, 0, 0)
        draw_sphere(0.5, earth_texture)
        glPopMatrix()

        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()

if __name__ == "__main__":
    main()
