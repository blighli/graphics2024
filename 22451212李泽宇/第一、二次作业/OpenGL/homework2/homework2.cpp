#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glut.h>
#include <cmath>
#include <iostream>

// 纹理ID
GLuint sunTexture, earthTexture, moonTexture;

// 存储球体信息
struct Sphere {
    GLfloat x, y, z, radius;
    const char* name;
};

// 定义太阳、地球和月亮的球体
Sphere sun = { 0.0f, 0.0f, 0.0f, 1.0f, "Sun" };
Sphere earth = { 3.0f, 0.0f, 0.0f, 0.5f, "Earth" };
Sphere moon = { 3.8f, 0.0f, 0.0f, 0.2f, "Moon" }; // 调整月亮的初始位置，确保月亮不被地球挡住

// 当前选中的球体名称
const char* currentObjectName = "";

// 纹理加载函数
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        std::cerr << "纹理加载失败: " << stbi_failure_reason() << std::endl;
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return texture;
}

// 初始化纹理和光源
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // 加载纹理
    sunTexture = loadTexture("pic/sun.jpg");
    earthTexture = loadTexture("pic/earth.jpg");
    moonTexture = loadTexture("pic/moon.jpg");
}

// 绘制球体并应用纹理
void drawSphere(GLfloat radius, GLuint texture) {
    GLUquadric* quad = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture);
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, radius, 50, 50);
    gluDeleteQuadric(quad);
}

// 绘制星球名字
void drawString(const char* str, GLfloat x, GLfloat y, GLfloat z) {
    glPushMatrix();
    glRasterPos3f(x, y, z);
    for (const char* c = str; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopMatrix();
}

// 绘制太阳、地球、月亮
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // 太阳
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    drawSphere(sun.radius, sunTexture);
    if (currentObjectName == sun.name) {
        drawString(sun.name, sun.x + sun.radius + 0.3f, sun.y, sun.z);  // 太阳名字显示在其右侧
    }
    glPopMatrix();

    // 地球
    glPushMatrix();
    glTranslatef(earth.x, earth.y, earth.z);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    drawSphere(earth.radius, earthTexture);
    if (currentObjectName == earth.name) {
        drawString(earth.name, earth.x , earth.y + earth.radius + 0.3f, earth.z );
    }
    glPopMatrix();

    // 月亮
    glPushMatrix();
    glTranslatef(moon.x, moon.y, moon.z);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    drawSphere(moon.radius, moonTexture);
    if (currentObjectName == moon.name) {
        drawString(moon.name, moon.x , moon.y+ earth.radius+0.3f, moon.z );  
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glutSwapBuffers();
}

// 设置窗口视口和投影矩阵
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
}

// 鼠标点击事件
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // 转换为OpenGL的坐标系
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        GLfloat winX, winY, winZ;
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        winY = (float)viewport[3] - (float)y;  // 转换y坐标
        glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        GLdouble posX, posY, posZ;
        gluUnProject(x, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // 检查是否点击到太阳、地球或月亮
        if (sqrt(pow(posX - sun.x, 2) + pow(posY - sun.y, 2) + pow(posZ - sun.z, 2)) < sun.radius) {
            currentObjectName = sun.name;
        }
        else if (sqrt(pow(posX - earth.x, 2) + pow(posY - earth.y, 2) + pow(posZ - earth.z, 2)) < earth.radius) {
            currentObjectName = earth.name;
        }
        else if (sqrt(pow(posX - moon.x, 2) + pow(posY - moon.y, 2) + pow(posZ - moon.z, 2)) < moon.radius) {
            currentObjectName = moon.name;
        }
        else {
            currentObjectName = "";
        }

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("太阳系模型");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
