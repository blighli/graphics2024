#include <GL/glut.h>
#include <math.h>

#define PI 3.14159265

static int year = 0, day = 0;

void init(void) {
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Sun - light source
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glPushMatrix();
    glColor3f(1.0, 1.0, 0.0);
    glutSolidSphere(1.0, 20, 16);   // Draw sun
    glPopMatrix();

    // Earth
    glPushMatrix();
    glRotatef((GLfloat) year, 0.0, 1.0, 0.0);
    glTranslatef(2.0, 0.0, 0.0);
    glRotatef((GLfloat) day, 0.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glutSolidSphere(0.2, 10, 8);    // Draw earth

    // Moon
    glRotatef((GLfloat) day, 0.0, 1.0, 0.0);
    glTranslatef(0.3, 0.0, 0.0);
    glColor3f(0.6, 0.6, 0.6);
    glutSolidSphere(0.05, 10, 8);   // Draw moon
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void timer(int value) {
    day = (day + 10) % 360;
    year = (year + 5) % 360;
    glutPostRedisplay();
    glutTimerFunc(100, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Solar System Simulation");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();
    return 0;
}
