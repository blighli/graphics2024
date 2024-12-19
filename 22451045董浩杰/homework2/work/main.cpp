#include "loadBMP.h"
#include "MouseEvent.h"

extern texture* sun, * earth, * moon;//纹理指针
extern int Xmouse, Ymouse;
extern char* selectedObject;
bool isRunning = true;

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);				//指定视口大小
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1, 20);		//透视投影

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	//指定照相机的位置	
}

void init(void)
{
	Reset();

	//清楚颜色
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//开启灯光效果
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	//启用二维纹理
	glEnable(GL_TEXTURE_2D);

	//光源设置
	GLfloat mat_shininess[] = { 10.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

	//启用阴影平滑
	glShadeModel(GL_SMOOTH);
	//设置材料反射属性
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//0号光源的位置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	//使用lmodel_ambien
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	//载入纹理
	LoadAllTextures();			
}




void idle()
{
	if (isRunning)  // 只有在运行状态下才更新
	{
		day += angle;
		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	if (key == ' ')  // 空格键切换运行状态
	{
		isRunning = !isRunning;  // 切换运行状态
	}
}

void display(void)
{
	//设置光源
	GLfloat  whiteLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat	 lightPos[] = { 0.0f, 0.0f, 0.0f, 2.0f };

	year = day / 365;
	month = day / 30;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat mat_ambient1[] = { 1,0,0,1 };
	GLfloat mat_emission[] = { 1,1,1,0 };

	GLfloat mat_ambient2[] = { 0.4,0.4,0.8,1 };
	GLfloat no_emission[] = { 0,0,0,1 };

	//绘制太阳
	glPushMatrix();
	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	glBindTexture(GL_TEXTURE_2D, sun->texID);
	glRotatef(month, 0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glRotatef(90, -1, 0, 0);

	glDisable(GL_LIGHTING);
	gltDrawSphere(1.0, 60, 40);	
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


	glPopMatrix();

	//绘制地球
	glPushMatrix();
	glRotatef((GLfloat)Yrot, 0., 1., 0.);
	glRotatef((GLfloat)Xrot, 1., 0., 0.);
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	//月亮与地球一起绕太阳转（地球公转）
	glRotatef(year, 0.0, 1.0, 0.0);				
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, earth->texID);
	glTranslatef(3.0, 0.0, 0.0);
	//地球自转 
	glRotatef(month, 0.0, 1.0, 0.0);			
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
	glRotatef(90, -1, 0, 0);
	gltDrawSphere(0.4, 40, 40);	
	glPopMatrix();

	//绘制月球
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, moon->texID);
	glTranslatef(3.0, 0.0, 0.0);
	glRotatef(60, -1, 1, 0);
	//月球绕地球转
	glRotatef(month, 0, 1.0, 0);				
	glTranslatef(0.6, 0.0, 0.0);
	glRotatef(90, -1, 0, 0);
	gltDrawSphere(0.07, 20, 20);
	glPopMatrix();
	glPopMatrix();

	glutSwapBuffers();
}





int main(int argc, char** argv)
{
	//初始化
	glutInit(&argc, argv);
	//初始化显示模式
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//初始化显示窗口大小
	glutInitWindowSize(1000, 600);
	//初始化显示窗口位置
	glutInitWindowPosition(100, 100);
	//创建窗口
	glutCreateWindow(argv[0]);
	
	//初始化用户配置
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);

	glutMainLoop();
	return 0;
}

