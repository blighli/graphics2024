# 期末大作业
## 要求
- 编写三维交互式动画程序
- [x] 使用OpenGL图形API
- [x] 实现了制作《Minecraft》（《我的世界》）的游戏原型

## 项目简介
本项目是一个使用Python开发的三维交互式动画程序，基于OpenGL图形API搭建了一个简化版的Minecraft游戏原型。项目使用了Pyglet库作为窗口管理和输入事件处理框架，支持基本的方块破坏与放置、玩家控制、世界渲染以及简单的菜单交互功能。



## 功能介绍

### 核心功能

1. **三维世界渲染**
   - 使用OpenGL渲染一个三维方块世界。
   - 支持按距离显示的动态渲染范围。

2. **玩家交互**
   - 支持玩家视角移动、方块破坏与放置。

3. **菜单界面**
   - 游戏主菜单，支持开始新游戏、加载游戏、查看控制说明等功能。
   - 暂停菜单，支持继续游戏、退出或保存退出。

4. **存档与加载**
   - 支持保存当前世界的玩家位置、视角、方块布局等信息，游戏存档会保存在项目目录下的`save.txt`文件中。
   - 支持加载先前保存的游戏进度。


---

## 按键说明

### 基本操作
- **W/A/S/D**: 移动
- **空格 (Space)**: 跳跃
- **左Ctrl**: 冲刺
- **左Shift**: 潜行
- **鼠标左键**: 破坏方块
- **鼠标右键**: 放置方块

### 视角与速度调整
- **鼠标移动**: 调整视角
- **+/-**: 增加或降低玩家移动速度
- **H**: 打开/关闭帮助菜单

### 菜单操作
- **ESC**: 打开暂停菜单或退出游戏

### 方块选择
- **1-7**: 选择不同类型的方块

---

## 游戏界面

### 主菜单
包含以下按钮：
1. **New Game**: 开始新游戏。
2. **Load Game**: 加载存档。
3. **Instructions**: 查看控制说明。

<div align="center">
  <img src="https://cdn.jsdelivr.net/gh/plutoky/My_PicGo/test/main_menu.png" alt="运行结果" height="450">
</div>


### 游戏界面
- 屏幕中心显示准星，用于瞄准操作。
- 按下鼠标左键破坏方块，右键放置方块，1-7切换方块类型。
  

<div align="center">
  <img src="https://cdn.jsdelivr.net/gh/plutoky/My_PicGo/test/interface.png" alt="运行结果" height="450">
</div>



### 暂停菜单
暂停菜单包含以下选项：
1. **Resume**: 继续游戏。
2. **Quit Without Saving**: 不保存直接退出。
3. **Save and Quit**: 保存并退出游戏。
   
<div align="center">
  <img src="https://cdn.jsdelivr.net/gh/plutoky/My_PicGo/test/pause_menu.png" alt="运行结果" height="450">
</div>

### 帮助菜单
详细列出所有按键操作说明。

<div align="center">
  <img src="https://cdn.jsdelivr.net/gh/plutoky/My_PicGo/test/instructions_menu.png" alt="运行结果" height="450">
</div>




