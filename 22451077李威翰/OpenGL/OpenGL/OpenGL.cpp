#include <gl/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex2f(-0.5f, -0.5f);  
    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex2f(0.5f, -0.5f);   
    glColor3f(0.0f, 0.0f, 1.0f); 
    glVertex2f(0.5f, 0.5f);  
    glColor3f(1.0f, 1.0f, 0.0f); 
    glVertex2f(-0.5f, 0.5f);  
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Triangle");


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
