import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import numpy as np
from PIL import Image
import pyrr

# 顶点着色器代码
VERTEX_SHADER = """
#version 330 core
layout(location = 0) in vec3 aPos;       // 顶点位置
layout(location = 1) in vec2 aTexCoord; // 纹理坐标
layout(location = 2) in vec3 aNormal;   // 法向量

out vec2 TexCoord;      // 传递纹理坐标到片段着色器
out vec3 FragPos;       // 片段位置
out vec3 Normal;        // 法向量

uniform mat4 model;     // 模型矩阵
uniform mat4 view;      // 视图矩阵
uniform mat4 projection;// 投影矩阵

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
"""

# 片段着色器代码
FRAGMENT_SHADER = """
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;      // 从顶点着色器传递的纹理坐标
in vec3 FragPos;       // 片段位置
in vec3 Normal;        // 法向量

uniform sampler2D texture1; // 纹理
uniform vec3 lightPos;      // 光源位置
uniform vec3 viewPos;       // 观察者位置
uniform vec3 lightColor;    // 光的颜色

void main()
{
    // 环境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 高光
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // 最终颜色
    vec3 result = (ambient + diffuse + specular) * texture(texture1, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
"""

# 球体顶点数据生成
def generate_sphere(radius, sectors, stacks):
    vertices = []
    normals = []
    texcoords = []
    indices = []

    for stack in range(stacks + 1):
        stack_angle = np.pi / 2 - stack * np.pi / stacks  # 从 pi/2 到 -pi/2
        xy = radius * np.cos(stack_angle)  # x 和 y 的投影
        z = radius * np.sin(stack_angle)  # z 位置

        for sector in range(sectors + 1):
            sector_angle = sector * 2 * np.pi / sectors  # 从 0 到 2pi

            x = xy * np.cos(sector_angle)
            y = xy * np.sin(sector_angle)
            vertices.extend([x, y, z])
            normals.extend([x / radius, y / radius, z / radius])
            texcoords.extend([sector / sectors, stack / stacks])

    for stack in range(stacks):
        for sector in range(sectors):
            first = stack * (sectors + 1) + sector
            second = first + sectors + 1
            indices.extend([first, second, first + 1])
            indices.extend([second, second + 1, first + 1])

    return np.array(vertices, dtype=np.float32), np.array(normals, dtype=np.float32), \
           np.array(texcoords, dtype=np.float32), np.array(indices, dtype=np.uint32)

# 加载纹理
def load_texture(path):
    texture = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture)

    # 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)

    # 加载图片
    image = Image.open(path).convert("RGB").transpose(Image.FLIP_TOP_BOTTOM)
    img_data = np.array(image, dtype=np.uint8)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data)
    glGenerateMipmap(GL_TEXTURE_2D)
    return texture

# 主程序
if not glfw.init():
    raise Exception("GLFW cannot be initialized!")

window = glfw.create_window(800, 600, "OpenGL Sphere with Texture", None, None)

if not window:
    glfw.terminate()
    raise Exception("GLFW window cannot be created!")

glfw.make_context_current(window)

# 编译着色器程序
shader = compileProgram(
    compileShader(VERTEX_SHADER, GL_VERTEX_SHADER),
    compileShader(FRAGMENT_SHADER, GL_FRAGMENT_SHADER)
)

# 生成球体数据
vertices, normals, texcoords, indices = generate_sphere(1.0, 72, 36)
vertices = np.hstack([vertices.reshape(-1, 3), texcoords.reshape(-1, 2), normals.reshape(-1, 3)]).flatten()
VBO = glGenBuffers(1)
VAO = glGenVertexArrays(1)
EBO = glGenBuffers(1)

glBindVertexArray(VAO)

# 顶点缓冲
glBindBuffer(GL_ARRAY_BUFFER, VBO)
glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)

# 元素缓冲
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO)
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.nbytes, indices, GL_STATIC_DRAW)

# 设置顶点属性
stride = (3 + 2 + 3) * 4
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(0))  # 顶点位置
glEnableVertexAttribArray(0)
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(12))  # 纹理坐标
glEnableVertexAttribArray(1)
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(20))  # 法向量
glEnableVertexAttribArray(2)

# 加载纹理
earth_texture = load_texture("earth.jpg")

# 启用深度测试
glEnable(GL_DEPTH_TEST)

# 摄像机和投影
view = pyrr.matrix44.create_look_at(pyrr.Vector3([0, 0, 5]), pyrr.Vector3([0, 0, 0]), pyrr.Vector3([0, 1, 0]))
projection = pyrr.matrix44.create_perspective_projection(45, 800 / 600, 0.1, 100)

# 光照参数
light_pos = pyrr.Vector3([2, 2, 2])
light_color = [1.0, 1.0, 1.0]

while not glfw.window_should_close(window):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    # 使用着色器
    glUseProgram(shader)

    # 设置 uniform
    model = pyrr.matrix44.create_from_translation([0, 0, 0])
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, model)
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, view)
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, projection)
    glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, light_pos)
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, [0, 0, 5])
    glUniform3fv(glGetUniformLocation(shader, "lightColor"), 1, light_color)

    # 绘制球体
    glBindTexture(GL_TEXTURE_2D, earth_texture)
    glBindVertexArray(VAO)
    glDrawElements(GL_TRIANGLES, len(indices), GL_UNSIGNED_INT, None)

    glfw.swap_buffers(window)
    glfw.poll_events()

glfw.terminate()
