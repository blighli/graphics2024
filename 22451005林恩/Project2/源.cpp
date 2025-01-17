#include <GL/glut.h>
#include <iostream>
#define GLUT_DISABLE_ATEXIT_HACK
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Test Window");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
