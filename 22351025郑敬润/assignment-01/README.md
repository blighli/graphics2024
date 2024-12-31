## 构建环境
> Tested on:
>
> - Ubuntu 22.04, g++ 11.4.0, NVIDIA Driver: 535.86.05, OpenGL: 4.5
> 
> - MacOS 13.6.1, Clang 15.0.0, M1 2020, OpenGL: 4.1
>
> - Windows10 22H2, MSVC2019 16.11.32, NVIDIA Driver: 536.23, OpenGL: 4.6
>

## Build

```shell
# Make sure CMake version 3.16+

# cd <project_dir>
cmake -S . -B build
cmake --build build
```

## Usage

- Render a triangle as default

```shell
# cd <project_dir>

# UNIX like:
./build/triangle
# Windows:
.\build\Debug\triangle.exe
```

## 3rd library

```shell
# ESSENTIAL
# Ubuntu:
sudo apt-get install build-essential libgl1-mesa-dev

# GLFW
# Obtained with CMake FetchContent; make sure github availability 

# GLAD
# (gl_ver: 4.6 Profile: Core) as internal using
# Order it in https://glad.dav1d.de/ 
# Replace ./src/glad.c ./include/glad/ ./include/KHR/
```
