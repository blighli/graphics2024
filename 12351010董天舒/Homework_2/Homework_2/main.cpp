#include <windows.h>  
#include <gl/glut.h>  
#include <math.h>  
#define GL_PI 3.1415f  


static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


GLfloat  whiteLight[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat  lightPos[] = { 0.0f,0.0f,0.0f,1.0f };

void RenderScene(void)
{
    static float fMoonRot = 0.0f;
    static float fEarthRot = 0.0f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -300.0f);

    //绘制太阳
    glColor3ub(255, 255, 0);
    glDisable(GL_LIGHTING);   //关闭光源  
    glutSolidSphere(20.0f, 20.0f, 20.0f);
    glEnable(GL_LIGHTING);    //启动光源
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


    //绘制地球  
    glColor3ub(0, 0, 255);
    glRotatef(fEarthRot, 0.0f, 1.0f, 0.0f);
    glTranslatef(105.0f, 0.0f, 0.0f);
    glutSolidSphere(15.0f, 15.0f, 15.0f);      //颜色，移动，旋转速度都是要在画之前决定的  


    //根据基于地球的坐标进行旋转，并绘制月球  
    glColor3ub(200, 200, 200);
    glRotatef(fMoonRot, 0.0f, 1.0f, 0.0f);
    glTranslatef(30.0f, 0.0f, 0.0f);
    fMoonRot += 15.0f;
    if (fMoonRot >= 365.0f)
        fMoonRot = 0.0f;
    glutSolidSphere(6.0f, 15.0f, 15.0f);  //旋转角度确定好后才画的月球。
    glPopMatrix();
    fEarthRot += 5.0f;
    if (fEarthRot >= 365.0f)
        fEarthRot = 0.0f;
    glutSwapBuffers();
}
void ChangeSize(GLsizei w, GLsizei h)
{
    GLfloat fAspect;
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    fAspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, fAspect, 1.0, 4000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SetupRC(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LIGHTING);                             //启动光源  
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight); //使用whiteLght所指定的环境光  
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);         //0号光源的位置  
    glEnable(GL_LIGHT0);
}

void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        xRot -= 5.0f;
    if (key == GLUT_KEY_DOWN)
        xRot += 5.0f;
    if (key == GLUT_KEY_LEFT)
        yRot -= 5.0f;
    if (key == GLUT_KEY_RIGHT)
        yRot += 5.0f;


    xRot = (GLfloat)((const int)xRot % 360);
    yRot = (GLfloat)((const int)yRot % 360);
    glutPostRedisplay();
}
void TimerFunc(int value)
{
    glutPostRedisplay();
    glutTimerFunc(100, TimerFunc, 1);
}
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Orthographic Projection");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);//设置上下左右键对图形旋转的控制 
    SetupRC();
    glutTimerFunc(250, TimerFunc, 1); //自动动画，计时器  
    glutMainLoop();
    return 0;
}