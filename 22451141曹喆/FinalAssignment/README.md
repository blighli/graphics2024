#### WebGPU中使用并行计算shader实现粒子运动

多线程并行计算。

模拟粒子运动和碰撞。

具体实现：

​	computepipeline实现粒子运动，先在computepipeline中计算粒子速度和位置（碰壁后反弹）和mvp矩阵（之前传统都是在js中计算），再在renderpipeline中绘制。