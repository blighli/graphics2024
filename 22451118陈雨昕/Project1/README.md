# 作业一

## 要求
- [x] 1.搭建OpenGL编程环境
- [x] 2.绘制简单图形

## 步骤
### 1.搭建OpenGL编程环境
**1.1 创建目录结构**
- `include/`: 包含头文件的目录（GLAD 和 GLFW 的头文件）。
- `lib/`: 包含链接库文件（`.dll`、`.lib` 文件）。

**1.2 配置第三方库**
- **配置 GLFW**：将库文件（如 `glfw3.lib`, `glfw3dll.lib`, `glfw3.dll`等）复制到项目的 `lib/` 目录中，并将 GLFW 头文件（`glfw3.h`）放入 `include/` 目录下。

- **配置 GLAD**：将GLAD 包中的 `glad.h`, `khrplatform.h` 文件放入 `include/` 目录下相应位置，并将 `glad.c` 文件放入项目根目录。
---

### 2. 配置项目属性
**2.1 包含目录**
- 在 **`VC++ 目录 > 包含目录`** 中添加以下路径：
  
  ```plaintext
  $(SolutionDir)include
  ```

**2.2 库目录**
- 在 **`VC++ 目录 > 库目录`** 中添加以下路径：
  
  ```plaintext
  $(SolutionDir)lib
  $(VC_LibraryPath_x64)
  $(WindowsSDK_LibraryPath_x64)
  ```


**2.3 附加依赖项**
- 在 **`链接器 > 常规 > 附加库目录`** 中确保添加库文件路径：
  
  ```plaintext
  $(SolutionDir)lib
  ```
- 在 **`链接器 > 输入 > 附加依赖项`** 中添加以下库文件：
  ```plaintext
  opengl32.lib
  glfw3.lib
  ```

---

### **3. 绘制简单图形**

- 编写能够绘制简单三角形的C++程序代码
- 点击生成解决方案，无错误即完成配置。
- 运行程序，出现以下窗口，说明运行成功。


<div align="center">
  <img src="https://cdn.jsdelivr.net/gh/plutoky/My_PicGo/test/%E8%BF%90%E8%A1%8C%E7%BB%93%E6%9E%9C.png" alt="运行结果" width="400">
</div>