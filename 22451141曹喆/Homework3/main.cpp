/// OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "parser.h"

using namespace std;
using namespace glm;

#define INF 1.0e10

// 函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void picking_callback(GLFWwindow *window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void interactionFunctions();

// 设置窗口大小
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// 相机
Camera camera(vec3(0.0f, 0.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true; // 判断鼠标是否刚进入第一模式, 用于更新光标禁用时鼠标的初始坐标

// 时间参数
float deltaTime = 0.0f;    // 也用于相机的调整速度
float lastFrame = 0.0f;

// 兔子参数
mat4 modelBunny = mat4(1.0f);    //四阶单位阵
GLuint vertices_size, indices_size;    //无符号整型 顶点以及面片size

// !!!: 光照参数
vec3 PointlightPos(15.0f, 15.0f, 15.0f);    //三维向量 光照

// 鼠标参数
GLfloat mouseX = SCR_WIDTH / 2.0;
GLfloat mouseY = SCR_HEIGHT / 2.0;
GLuint selectedPointIndice = 0;  // max

const int MAXPOINT = 40000;
const int MAXINDEX = 70000;

const char* normalFile = "bunny_normal.ply2";
const char *SPLIT = "--------------------------------------------------------------";
GLfloat vertices[MAXPOINT*6];
GLuint indices[MAXINDEX*3];

bool keys[1024];    // 存取键码,判断按键情况
bool isAttenuation = false; // 判断光照是否衰减
bool isFlashlight = false;  // 判断是否打开闪光灯
bool cursorDisabled = true; // 判断光标是否被禁用

void description(){
    cout << SPLIT << std::endl;
    cout << "Starting GLFW context, OpenGL 3.3\n";
    cout << "=: 点光源衰减\n";
    cout << "ENTER: 光照开关\n";
    cout << "A: 相机向左.\n";
    cout << "D: 相机向右.\n";
    cout << "W: 相机向前\n";
    cout << "S: 相机向后.\n";
    cout << "方向键<-: 兔子左移.\n";
    cout << "方向键->: 兔子右移.\n";
    cout << "J: 兔子向左旋转.\n";
    cout << "L: 兔子向右旋转.\n";
    cout << "I: 兔子向前旋转.\n";
    cout << "K: 兔子向后旋转\n";
    cout << "↑或鼠标向上滚动: 兔子放大\n";
    cout << "↓或鼠标向下滚动: 兔子缩小\n";
    cout << SPLIT << std::endl;
}

//窗口初始化函数
GLFWwindow* init(){
    glfwInit();//初始化glfw库
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//主版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//次版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw 窗口创建
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Stanford bunny", NULL, NULL);
    
    //如果窗口创建失败，返回失败信息。
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();    //释放所有资源
        exit(EXIT_SUCCESS);
    }
    
    glfwMakeContextCurrent(window);    // 设置当前的窗口上下文
    //注册函数，该函数还可以注册其他相关硬件的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //设置按键回调函数
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);    //设置光标回调函数
    glfwSetMouseButtonCallback(window, picking_callback);    //设置鼠标按键回调函数
    glfwSetScrollCallback(window, scroll_callback);        //设置滚轮回调函数
    
    // 让GLFW捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: 加载所有openGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_SUCCESS);
    }
    
    // 配置opengl的全局状态: 开启深度测试
    glEnable(GL_DEPTH_TEST);
    return window;
}

int main(){
    // glfw: 初始化并且配置相关参数
    
    // 输出按键说明
    description();
    
    //调用窗口初始化函数
    GLFWwindow* window = init();
    
    //建立并编译着色器
    Shader lightingShader("bunny.vs", "bunny.fs");    //vs文件用来处理顶点 fs文件用来处理像素
    Shader lightShader("light.vs", "light.fs");        //灯着色器
    Shader selectShader("select.vs", "select.fs");    //拾取着色器
    
    // 设置顶点数据（和缓冲区）并配置顶点属性
    // 导入ply2文件
    parseNormal(normalFile, vertices, indices, vertices_size, indices_size);
//    parseIndex(normalFile, vertices, indices, vertices_size, indices_size);
    
    /*
     //test code:
     cout << "测试是否读取正确:" << endl;
     for (int i = 0; i < 3; i++)
     {
     cout << vertices[6*i] << " " << vertices[6*i+1] << " " << vertices[6*i+2] << " " << vertices[6*i+3] << " " << vertices[6*i+4] << " " << vertices[6*i+5] << std::endl;
     }
     cout << SPLIT << endl;
     */
    
    // 灯光顶点数据
    float lightVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        
        0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
        
        //        -0.2f, -0.2f, -0.2f,  0.2f, -0.2f, -0.2f,
        //        0.2f,  0.2f, -0.2f,  0.2f,  0.2f, -0.2f,
        //        -0.2f,  0.2f, -0.2f, -0.2f, -0.2f, -0.2f,
        //        -0.2f, -0.2f,  0.2f,  0.2f, -0.2f,  0.2f,
        //        0.2f,  0.2f,  0.2f,  0.2f,  0.2f,  0.2f,
        //        -0.2f,  0.2f,  0.2f, -0.2f, -0.2f,  0.2f,
        //        -0.2f,  0.2f,  0.2f, -0.2f,  0.2f, -0.2f,
        //        -0.2f, -0.2f, -0.2f, -0.2f, -0.2f, -0.2f,
        //        -0.2f, -0.2f,  0.2f, -0.2f,  0.2f,  0.2f,
        //        0.2f,  0.2f,  0.2f,  0.2f,  0.2f, -0.2f,
        //        0.2f, -0.2f, -0.2f,  0.2f, -0.2f, -0.2f,
        //        0.2f, -0.2f,  0.2f,  0.2f,  0.2f,  0.2f,
        //        -0.2f, -0.2f, -0.2f,  0.2f, -0.2f, -0.2f,
        //        0.2f, -0.2f,  0.2f,  0.2f, -0.2f,  0.2f,
        //        -0.2f, -0.2f,  0.2f, -0.2f, -0.2f, -0.2f,
        //        -0.2f,  0.2f, -0.2f,  0.2f,  0.2f, -0.2f,
        //        0.2f,  0.2f,  0.2f,  0.2f,  0.2f,  0.2f,
        //        -0.2f,  0.2f,  0.2f, -0.2f,  0.2f, -0.2f
    };
    
    // 第一步，配置兔子的VAO(和VBO)
    unsigned int bunnyVAO, bunnyVBO, bunnyEBO;
    glGenVertexArrays(1, &bunnyVAO);    // 创建一个顶点数组（Vertex Array Object）对象
    glGenBuffers(1, &bunnyVBO);    // 申请显存空间，分配顶点缓冲对象（Vertex Buffer Object）的ID
    glGenBuffers(1, &bunnyEBO);    // 创建索引缓冲对象（Element Buffer Object）
    
    glBindVertexArray(bunnyVAO);      // 绑定VAO对象
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);    // 绑定VBO
    glBufferData(GL_ARRAY_BUFFER, 6*vertices_size*sizeof(GLfloat), vertices, GL_STATIC_DRAW);   // 将用户定义的顶点数据传输到绑定的显存缓冲区中
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyEBO);    // 绑定EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * indices_size * sizeof(GLuint), indices, GL_STATIC_DRAW);    // 将索引存储到EBO当中
    
    /*
     glVertexAttribPointer()
     第一个参数指定顶点属性位置，与顶点着色器中layout(location=0)对应。
     第二个参数指定顶点属性大小。
     第三个参数指定数据类型。
     第四个参数定义是否希望数据被标准化。
     第五个参数是步长（Stride），指定在连续的顶点属性之间的间隔。
     第六个参数表示我们的位置数据在缓冲区起始位置的偏移量
     */
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);    //通知OPENGL怎么解释这些顶点数据
    glEnableVertexAttribArray(0);    //开启顶点属性
    
    // 普通属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 请注意，这是允许的，对glVertexAttribPointer的调用将VBO注册为当前绑定的顶点缓冲区对象，因此之后我们可以安全地取消绑定
    glBindVertexArray(0); // 取消绑定VAO（取消绑定任何缓冲区/数组以防止出现奇怪的错误始终是一件好事），请记住：请勿取消绑定EBO，继续将其与VAO绑定
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //解除绑定VAO后解除EBO绑定。
    

    // 其次，配置灯光的VAO（和VBO）
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);    //绑定缓存
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);    //灯光顶点数据绑定到缓存中
    glBindVertexArray(lightVAO);    //绑定灯光VAO对象
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);    //通知OPENGL处理顶点数据
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 请注意，这是允许的，对glVertexAttribPointer的调用将VBO注册为当前绑定的顶点缓冲区对象，因此之后我们可以安全地取消绑定
    glBindVertexArray(0); // 取消绑定VAO（取消绑定任何缓冲区/数组以防止出现奇怪的错误始终是一件好事），请记住：请勿取消绑定EBO，继续将其与VAO绑定
    
    //兔子的初始状态(第二个参数为初始角度,radians设置弧度）
    modelBunny = rotate(modelBunny, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
    
    // 渲染循环
    while (!glfwWindowShouldClose(window)){    //glfwWindowShouldClose 检查窗口是否需要关闭
        
        // 每帧的时间逻辑
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // 输入
        interactionFunctions();
        
        // !!!: 设置初始模式
        if(!cursorDisabled) //如果光标没有禁用
            // 显示物体所有面，显示线段，多边形用轮廓显示
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            //否则 显示面，多边形采用填充式
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // !!!: 渲染背景颜色
        glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清除颜色缓冲以及深度缓冲
        
        // 设置格式/绘图对象时一定要激活着色器
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        
        // 灯光特性
        vec3 lightColor;
        
        // !!!: 颜色按时改变
        /*
         //兔子颜色变换，根据glfwGetTime获取时间
         lightColor.x = sin(glfwGetTime() * 2.0f);
         lightColor.y = sin(glfwGetTime() * 0.7f);
         lightColor.z = sin(glfwGetTime() * 1.3f);
         */
        
        // !!!: 设置光照属性
        lightColor.x = 4.0f;   // 0.0f
        lightColor.y = 4.0f;    // 0.0f
        lightColor.z = 0.0f;
        vec3 diffuseColor = lightColor   * vec3(0.3f); // 减少影响
        vec3 ambientColor = diffuseColor * vec3(0.2f); // 低影响
        
        // 点光源
        lightingShader.setVec3("pointLights.position", PointlightPos);
        lightingShader.setVec3("pointLights.ambient", ambientColor);
        lightingShader.setVec3("pointLights.diffuse", diffuseColor);
        lightingShader.setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights.constant", 1.0f);
        lightingShader.setFloat("pointLights.linear", (isAttenuation ? 0.014 : 0.0f));
        lightingShader.setFloat("pointLights.quadratic", (isAttenuation ? 0.0007 : 0.0f));
        
        // 聚光灯
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 5.0f, 5.0f, 5.0f);  //环境光
        lightingShader.setVec3("spotLight.diffuse", 5.0f, 5.0f, 5.0f);  //漫反射光
        lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f); //镜面高光
        lightingShader.setFloat("spotLight.constant", (isFlashlight? 1.0f : INF));
        lightingShader.setFloat("spotLight.linear", 0.045f);
        lightingShader.setFloat("spotLight.quadratic", 0.0075f);
        lightingShader.setFloat("spotLight.cutOff", cos(radians(5.0f)));
        lightingShader.setFloat("spotLight.outerCutOff", cos(radians(9.0f)));
        
        // 材料特性
        lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);  //环境光
//        lightingShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);    // 环境光
        lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.2f);    //漫反射光
        lightingShader.setVec3("material.specular", 1.0f, 1.0f, 1.0f); //    镜面光不会完全影响该物体的材质(高光)
        lightingShader.setFloat("material.shininess", 320.0f);
        
        // 查看/投影转换
        mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        mat4 view = camera.GetViewMatrix();    //获取相机的视野矩阵
        lightingShader.setMat4("projection", projection);    //将投影矩阵传入到着色器
        lightingShader.setMat4("view", view);    //相机视野矩阵传入着色器
        
        // 世界坐标转换
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", modelBunny); // 兔子模型参数传入到model着色器
        
        // 渲染兔子
        glBindVertexArray(bunnyVAO);    //绑定兔子的VAO
        glDrawElements(GL_TRIANGLES, 3*indices_size, GL_UNSIGNED_INT, 0);//图元绘制函数
        glBindVertexArray(0);    //取消绑定
        
        // !!!: 画出灯的对象
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        mat4 model = mat4(1.0f);
        model = translate(model, PointlightPos);
        model = scale(model, vec3(0.4f)); // 一个较小的立方体（灯源）
        lightShader.setMat4("model", model);
        
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);    //根据顶点数组中的坐标数据和指定的模式，进行绘制
        glBindVertexArray(0);//取消绑定
        
        // !!!: 拾取点的坐标
        if (selectedPointIndice <= vertices_size){    //如果点的索引小于点的个数
            selectShader.use();    //启用着色器
            
            //创建一个四维向量记录当前点的位置
            //n1 n2 n3 分别为该点的xyz坐标
            vec4 now(modelBunny * vec4(vertices[selectedPointIndice * 6], vertices[selectedPointIndice * 6 + 1], vertices[selectedPointIndice * 6 + 2], 1.0f));
            
            // 创建一个平移矩阵，第一个参数为目标矩阵，第二个矩阵是平移的方向向量s
            model = translate(mat4(1.0f), vec3(now.x, now.y, now.z));
            // model是一个小立方体，scale用于创建一个缩放矩阵
            model = scale(model, vec3(0.01f, 0.01f, 0.01f));
            view = camera.GetViewMatrix();    // 视野矩阵
            projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);    // 查看矩阵
            
            // 着色器设置相关矩阵参数
            selectShader.setMat4("projection", projection);
            selectShader.setMat4("view", view);
            selectShader.setMat4("model", model);
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);    //画出一个小立方体框住所选的点。 函数说明：根据顶点数组中的坐标数据和指定的模式（三角形）进行绘制
        }
        
        // glfw: 交换缓冲区和轮询IO事件（按下/释放键，鼠标移动等）
        glfwSwapBuffers(window); // 交换缓冲区,使用双缓冲（让这个小方块显示在屏幕上）;
        glfwPollEvents();
    }
    
    // optional: 一旦实现了目标取消所有资源分配
    glDeleteVertexArrays(1, &bunnyVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &bunnyVBO);
    glDeleteBuffers(1, &lightVBO);
    
    // glfw: 最后一步，释放所有已分配的GLFW资源。
    glfwTerminate();
    return 0;
}

// 输入函数
void interactionFunctions(){
    // 调节旋转/缩放速度
    GLfloat bunnySpeed = 30.0f * deltaTime;
    // 相机向前
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, bunnySpeed);
    // 相机向后
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, bunnySpeed);
    // 相机向左
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, bunnySpeed);
    // 相机向右
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, bunnySpeed);
    // <- 兔子向左移动
    if (keys[GLFW_KEY_LEFT])
        modelBunny = translate(modelBunny, vec3(bunnySpeed, 0, 0));
    // -> 兔子向右移动
    if (keys[GLFW_KEY_RIGHT])
        modelBunny = translate(modelBunny, vec3(-bunnySpeed, 0, 0));
    // J 兔子向左转
    if (keys[GLFW_KEY_J])
        modelBunny = rotate(modelBunny, radians(bunnySpeed), vec3(0.f, 0.f, 1.f));
    // L 兔子向右转
    if (keys[GLFW_KEY_L])
        modelBunny = glm::rotate(modelBunny, glm::radians(-bunnySpeed), glm::vec3(0.f, 0.f, 1.f));
    // I 兔子向前转
    if (keys[GLFW_KEY_I])
        modelBunny = glm::rotate(modelBunny, glm::radians(-bunnySpeed), glm::vec3(1.f, 0.f, 0.f));
    // K 兔子向后转
    if (keys[GLFW_KEY_K])
        modelBunny = glm::rotate(modelBunny, glm::radians(bunnySpeed), glm::vec3(1.f, 0.f, 0.f));
    // ↓键 兔子缩小
    if (keys[GLFW_KEY_DOWN])
        modelBunny = glm::scale(modelBunny, glm::vec3(1.0f - 0.1f * bunnySpeed, 1.0f - 0.1f * bunnySpeed, 1.0f - 0.1f * bunnySpeed));
    // ↑键 兔子放大
    if (keys[GLFW_KEY_UP])
        modelBunny = glm::scale(modelBunny, glm::vec3(1.0f + 0.1f * bunnySpeed, 1.0f + 0.1f * bunnySpeed, 1.0f + 0.1f * bunnySpeed));
}

//处理所有输入：查询GLFW是否在此帧中按下/释放了相关的按键并做出相应的反应
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode){
    // 如果按下“=”键
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS){
        isAttenuation = !isAttenuation; // 将光照是否衰减取反
        cout << "点光源衰减: ";
        if(isAttenuation)
            cout << "打开.\n";
        else
            cout << "关闭.\n";
        cout << SPLIT << endl;
    }
    
    // 按下Enter键，打开闪光灯
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        isFlashlight = !isFlashlight;
        cout << "闪光灯： ";
        if(isFlashlight)
            cout << "打开.\n";
        else
            cout << "关闭.\n";
        cout << SPLIT << std::endl;
    }
    
    // 按下esc关闭窗口
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    
    // 按下tab键切换模式
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS){
        //获取当前的输入模式，判断光标是否禁用
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED){
            // 如果禁用，则恢复使用光标
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorDisabled = false;    //禁用标志
        }else{
            //如果当前光标不被禁用，则禁用光标（离开拾取模式）
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorDisabled = true;
            firstMouse = true;  // 记录鼠标返回第一视角模式
        }
    }
    
    // 判断所有按键的按下与松开状态
    if (key >= 0 && key < 1024){
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

// glfw: 每当改变窗口大小时，就会执行回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // 确保视口与新的窗口尺寸匹配，请注意，窗口的宽度和高度将大于视口在显示屏上的指定的宽和高
    glViewport(0, 0, width, height);
}

// glfw: 任何时候移动鼠标调用该回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    mouseX = xpos;    //鼠标x坐标
    mouseY = ypos;    //鼠标y坐标
    
    if(cursorDisabled){    // 如果光标禁用
        if (firstMouse){    // 如果鼠标第一视角可用更新当前xy坐标
            lastX = xpos;
            lastY = ypos;
//            cout << lastX << "  " << lastY << endl;
            firstMouse = false;
        }
        
        //计算x y坐标的偏移量
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;  // 反转，在Y坐标轴上，从下到上
        lastX = xpos;
        lastY = ypos;
        camera.ProcessMouseMovement(xoffset, yoffset);    // 相机根据坐标偏移量移动
    }
}

// glfw: 任何时候鼠标滚轮滚动时都会调用该函数
// 根据滚轮的滑动，调节相机视图的zoom参数来进行放大缩小
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}

// !!!: 拾取点坐标
void picking_callback(GLFWwindow *window, int button, int action, int mods){
    // bool test = false;
    
    // 判断光标是否没有被禁用 鼠标左键是否被按下
    if (!cursorDisabled && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_MOUSE_BUTTON_LEFT){
        
        // 鼠标的xy坐标
        GLfloat xpos = mouseX;
        GLfloat ypos = mouseY;
        cout << "🤠选取点的屏幕坐标(窗口的左上角为0,0): (" << xpos << "," << ypos << ")" <<endl;
        
        GLfloat minDistance = pow(10, 20);    // 最小距离10^20
        GLuint minIndice = 0;    // 最小索引
        GLfloat minX =  0.0f, minY = 0.0f;
        for (int i = 0; i < vertices_size; i++){
            /*
             将modelBunny的矩阵翻转，然后转置将结果变为一个三阶矩阵 * 点的颜色值 点乘 相机的前坐标 <0 及这个点在正视图上
             判断哪些点应该被拾取
             */
            if (dot(mat3(transpose(inverse(modelBunny))) *
                    vec3(vertices[6 * i + 3], vertices[6 * i + 4], vertices[6 * i + 5]), camera.Front) < 0)
            {
                //std::cout << vertices[6 * i + 3] << vertices[6 * i + 4] << vertices[6 * i + 5] << std::endl;
                //std::cout << glm::vec3(vertices[6 * i + 3], vertices[6 * i + 4], vertices[6 * i + 5]).x << glm::vec3(vertices[6 * i + 3], vertices[6 * i + 4], vertices[6 * i + 5]).y<< glm::vec3(vertices[6 * i + 3], vertices[6 * i + 4], vertices[6 * i + 5]).z << std::endl;
                vec4 iPos;
                mat4 view = camera.GetViewMatrix();    // 相机的视野矩阵
                
                mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);    //投影矩阵,   perspectives是投影矩阵公式 radians是弧度
                
                iPos = modelBunny * vec4(vertices[i * 6], vertices[i * 6 + 1], vertices[i * 6 + 2], 1.0f);
                iPos = projection * view * iPos;
                
                // 屏幕点的x坐标
                GLfloat pointPosX = SCR_WIDTH / 2 * (iPos.x / iPos.w) + SCR_WIDTH / 2;
                // 点的y坐标（特别注意这里的负号）
                GLfloat pointPosY = SCR_HEIGHT / 2 * (-iPos.y / iPos.w) + SCR_HEIGHT / 2;
                
                // 判断当前点与所有应该拾取的点之间的距离，并不断的更新minDistance直到所有的点被计算完，得出光标点击的点与离它最近的点之前的距离记录为minDistance
                if ((pointPosX - xpos) * (pointPosX - xpos) + (pointPosY - ypos) * (pointPosY - ypos) < minDistance) {
                    minDistance = (pointPosX - xpos) * (pointPosX - xpos) + (pointPosY - ypos) * (pointPosY - ypos);
//                    cout << minDistance << " ";
                    minIndice = i;
                    minX = pointPosX;
                    minY = pointPosY;
                }
            }
        }
        // 如果minDistance小于20个像素
        if (minDistance < 400){
            selectedPointIndice = minIndice;
            cout << "点的索引号: " << minIndice << endl;
            cout << "点的坐标: (" << vertices[minIndice * 6] << "," << vertices[minIndice * 6 + 1] << "," << vertices[minIndice * 6 + 2] << ")" << endl;
            cout << "点的屏幕坐标: (" << minX << "," << minY << ")" << endl;
        }else{
            cout << "附近没有点(光标与最近点之间的距离必须小于20个像素)" << endl;
        }
        cout << SPLIT << endl;
    }
}
