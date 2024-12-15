#include <gl\glut.h> 
#include "math.h"
#include <iostream>
/*
实验一：
完成基本图形的绘制

*/
using namespace std;

int n = 3600;  //圆绘制次数
float PI = 3.1415926f;
float R = 0.1f;  //半径

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glColor4f(0, 0, 1, 0);



	
	//橘黄色画填充面
	glColor3f(1.0, 0.5, 0.25);
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.5, 0.25);
	glVertex2f(-0.1, 0.1);
	glVertex2f(-0.2, 0.1);
	glColor3f(1.0, 1.0, 0.25);
	glVertex2f(-0.3, 0.3);
	glVertex2f(-0.4, 0.5);
	glColor3f(1.0, 0.5, 0.25);
	glVertex2f(-0.5, 0.3);
	glEnd();




	//画圆
	glColor4f(0, 0, 1, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex2f(R * cos(2 * PI * i / n), R * sin(2 * PI * i / n));   //定义顶点
	}
	glEnd();
	glFlush();


}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);   //初始化GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("作业一：使用OpenGL绘制一个简单图形");
	glutDisplayFunc(&myDisplay);   //回调函数 
	glutMainLoop();    //持续显示，当窗口改变会重新绘制图形

}
