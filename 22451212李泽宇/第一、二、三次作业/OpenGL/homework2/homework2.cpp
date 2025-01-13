#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glut.h>
#include <cmath>
#include <iostream>

// ����ID
GLuint sunTexture, earthTexture, moonTexture;

// �洢������Ϣ
struct Sphere {
    GLfloat x, y, z, radius;
    const char* name;
};

// ����̫�������������������
Sphere sun = { 0.0f, 0.0f, 0.0f, 1.0f, "Sun" };
Sphere earth = { 3.0f, 0.0f, 0.0f, 0.5f, "Earth" };
Sphere moon = { 3.8f, 0.0f, 0.0f, 0.2f, "Moon" }; // ���������ĳ�ʼλ�ã�ȷ��������������ס

// ��ǰѡ�е���������
const char* currentObjectName = "";

// ������غ���
GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        std::cerr << "�������ʧ��: " << stbi_failure_reason() << std::endl;
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image);

    // �����������
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return texture;
}

// ��ʼ������͹�Դ
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

    // ��������
    sunTexture = loadTexture("pic/sun.jpg");
    earthTexture = loadTexture("pic/earth.jpg");
    moonTexture = loadTexture("pic/moon.jpg");
}

// �������岢Ӧ������
void drawSphere(GLfloat radius, GLuint texture) {
    GLUquadric* quad = gluNewQuadric();
    glBindTexture(GL_TEXTURE_2D, texture);
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, radius, 50, 50);
    gluDeleteQuadric(quad);
}

// ������������
void drawString(const char* str, GLfloat x, GLfloat y, GLfloat z) {
    glPushMatrix();
    glRasterPos3f(x, y, z);
    for (const char* c = str; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopMatrix();
}

// ����̫������������
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // ̫��
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    drawSphere(sun.radius, sunTexture);
    if (currentObjectName == sun.name) {
        drawString(sun.name, sun.x + sun.radius + 0.3f, sun.y, sun.z);  // ̫��������ʾ�����Ҳ�
    }
    glPopMatrix();

    // ����
    glPushMatrix();
    glTranslatef(earth.x, earth.y, earth.z);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    drawSphere(earth.radius, earthTexture);
    if (currentObjectName == earth.name) {
        drawString(earth.name, earth.x , earth.y + earth.radius + 0.3f, earth.z );
    }
    glPopMatrix();

    // ����
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

// ���ô����ӿں�ͶӰ����
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
}

// ������¼�
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // ת��ΪOpenGL������ϵ
        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        GLfloat winX, winY, winZ;
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        winY = (float)viewport[3] - (float)y;  // ת��y����
        glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        GLdouble posX, posY, posZ;
        gluUnProject(x, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // ����Ƿ�����̫�������������
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
    glutCreateWindow("̫��ϵģ��");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
