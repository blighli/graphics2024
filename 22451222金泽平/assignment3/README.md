# 三维场景渲染项目

## 项目描述
本项目展示了一个三维场景，其中包含一个模拟的兔子模型。利用 OpenGL 和 GLSL，本项目演示了如何加载和渲染三维模型，支持多个光源的光照效果,实现了模型旋转缩放，和以视点为中心的场景漫游。

## 环境配置

### 开发环境
请确保你的开发环境中安装有 OpenGL 和 GLSL 的支持。

### 依赖库
项目依赖于以下库，请确保已正确安装：
- GLFW
- GLEW
- GLM
- Assimp

## 运行程序
在生成的可执行文件目录下运行程序。确保模型文件路径在执行时指定正确。

## 功能描述
模型加载：使用 Assimp 库加载 OBJ 格式的三维模型。
光照效果：实现了基础的 Phong 光照模型，支持多个光源的光照效果。
实现以模型为中心的的平移旋转和缩放
实现以视点为中心的场景漫游

## 项目结构
src/：包含所有源代码文件。
assets/：包含三维模型和纹理等静态资源。
include/：包含项目所需的所有头文件。
shaders/：存放 GLSL 着色器代码。

![示例](https://github.com/Kswink/graphics2024/blob/master/22451222%E9%87%91%E6%B3%BD%E5%B9%B3/assignment3/%E6%A8%A1%E5%9E%8B%E5%9B%BE%E4%B8%80.png?raw=true)

![示例](https://github.com/Kswink/graphics2024/blob/master/22451222%E9%87%91%E6%B3%BD%E5%B9%B3/assignment3/%E6%A8%A1%E5%9E%8B%E5%9B%BE%E4%BA%8C.png?raw=true)

