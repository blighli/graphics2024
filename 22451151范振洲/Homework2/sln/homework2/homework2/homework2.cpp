#include <gl/glut.h>
#include <math.h>
#include<gl/glaux.h>

float light_angle = 0;//光源角度
float light_radius = 8.0;//光源半径

static float year = 0, month = 0, day = 0;
double aix_x = 0.0, aix_y = 1, aix_z = 0;
GLuint texName;//纹理名称
GLUquadricObj* qobj;//二次曲面声明类型

void IPosition(void);// 光源位置
void init(void);// 初始化，启动光源、材质、消隐
void material_sun(void);//太阳材质
void material_earth(void);// 地球材质
void material_moon(void);// 月球材质
void sun(void);//绘制太阳
void earth(void);//绘制地球、月球
void display(void);
void Rotate(void);   //更新旋转参数
void myidle(void);   //闲置调用函数
void reshape(int w, int h);
void mykeyboard(unsigned char key, int x, int y);
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("作业二");
	init();
	glutDisplayFunc(display);   
	glutReshapeFunc(reshape);   
	glutKeyboardFunc(mykeyboard); 
	glutIdleFunc(myidle);      
	glutMainLoop();
	return 0;
}
//设置光源位置
void IPosition()
{
	float y, z;
	y = light_radius * cos(light_angle);
	z = light_radius * sin(light_angle);
	float light_position[] = { 3.0,y,z,0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//启动光源、消隐、材料
void init(void)
{
	IPosition(); //光源位置
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);//使用颜色材质
	glClearDepth(1.0f);
	AUX_RGBImageRec* TextureImage[1];//加载位图
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //多边形的显示方式,模式将适用于物体的所有面采用填充形式
	glGenTextures(1, &texName);//glGenTextures (GLsizei n, GLuint *textures);在数组textures中返回n个当期未使用的值,表示纹理对象的名称 
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//纹理过滤函数，必须写两遍
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	memset(TextureImage, 0, sizeof(void*) * 1);//作用是将某一块内存中的内容全部设置为指定的值
	if (TextureImage[0] = auxDIBImageLoadA("13.bmp"))   //实现加载位图文件
	{
		//根据指定的参数，生成一个2D纹理（Texture）。
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	glMatrixMode(GL_PROJECTION);
}
void myidle(void)
{
	glutPostRedisplay();
}
void display(void)
{
	IPosition();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Rotate();
	sun();
	earth();
	glutSwapBuffers();
	glFlush();
}
//太阳材质
void material_sun(void)
{
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数
	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数
	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 
	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };
	GLfloat lmodel_ambient[] = { 1.0,0.0,0.0,1.0 };       //太阳颜色为红色     
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色
	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度
}
//地球材质
void material_earth(void)
{
	glEnable(GL_COLOR_MATERIAL);
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数
	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数
	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 
	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.0,0.0,1.0,1.0 };       //地球颜色为蓝色     
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色
	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度
}
//月球材质
void material_moon(void)
{
	glEnable(GL_COLOR_MATERIAL);
	GLfloat mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };      //材质镜面反射颜色参数
	GLfloat mat_shininess[] = { 50.0 };                   // 镜面反射指数参数
	GLfloat mat_diffuse[] = { 0.0,0.0,1.0,1.0 };           //材质散射颜色	 
	GLfloat white_light[] = { 1.0, 0.0,0.0, 1.0 };

	GLfloat lmodel_ambient[] = { 0.5,0.5,0.5,1.0 };       //月亮颜色为灰色     


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   //材质镜面反射颜色
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); //镜面反射指数
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  //材质的散射颜色
	glMaterialfv(GL_FRONT, GL_AMBIENT, white_light);     //材质的环境颜色 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);  //  整个场景的环境光的RGBA强度
}
//绘制太阳
void sun()
{
	qobj = gluNewQuadric();//申请空间
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texName);//允许建立一个绑定到目标纹理的有名称的纹理。
	glEnable(GL_TEXTURE_2D);//启用二维纹理
	gluQuadricTexture(qobj, GL_TRUE);//纹理函数
	//material_sun();
	glRotatef((GLfloat)year, 0.0, 1.0, 0.0);   //太阳自转	
	//glutSolidSphere(10, 200, 200);                //画球体   
	gluSphere(qobj, 0.65, 60, 60); //二次曲面qobj
	glDisable(GL_TEXTURE_2D);//禁用二维纹理
	glPopMatrix();
}
//绘制地球和月球
void earth()
{
	glPushMatrix();
	material_earth();
	//glTranslatef((GLfloat)year, 0, 1, 0);
	glRotatef((GLfloat)day, aix_x, aix_y, aix_z);//公转
	glTranslatef(1.0, 0.0, 0.0);//平移
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//自转
	//地球绕太阳的公转 设置//绘制地球
	glutSolidSphere(0.1, 20, 16);
	//绘制月球
	material_moon();
	glRotatef((GLfloat)day, 0, 1, 0);//公转
	glTranslatef(0.25, 0, 0);
	glutSolidSphere(0.05, 20, 16);
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);//自转
	glPopMatrix();
}
void Rotate(void)          //设置各行星的公转周期
{
	year = year + 0.2;
	if (year > 360.0)
	{
		year = year - 360.0;
	}
	day = day + 0.2;
	if (day > 360.0)
	{
		day = day - 360.0;
	}
	month = month + 0.5;
	if (month > 360)
		month -= 360;
	glutPostRedisplay();

}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
	//glOrtho(-30.0, 30.0, -30.0, 30.0, -30.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void mykeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'W':
	case 'w':
		aix_y -= 0.01;
		aix_z += 0.01;
		break;
	case 'S':
	case 's':
		aix_y += 0.01;
		aix_z -= 0.01;
		break;
	case 'A':
	case 'a':
		light_angle -= 1.0;
		break;
	case 'D':
	case 'd':
		light_angle += 1.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}