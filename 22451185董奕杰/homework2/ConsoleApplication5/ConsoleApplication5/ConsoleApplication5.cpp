#ifndef _HERDER_H_

#include <glut.h>
#include <glaux/glaux.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

AUX_RGBImageRec* Images[3];
GLuint ImagesIDs[4];
const char* szFiles[3] = {
	(".\\src\\sun.bmp"),
	(".\\src\\moon.bmp"),
	(".\\src\\earth.bmp"), };


#define PI 3.1415926
static float  year = 0, month = 0, day = 0, angle = 30;

GLint W, H, width, height;
float pox = 2, poy = 3, poz = 8;  //�������λ��
GLint fovy = 60;
GLubyte* pImg;
GLint iWidth, iHeight;

void Init();
void OnDisplay();
void OnReshape(int, int);
void OnTimer(int);
void DrawCircle(GLdouble);
void gltDrawSphere(GLfloat, GLint, GLint);

#endif

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("̫��ϵ");
	Init();
	glutReshapeFunc(OnReshape);
	glutDisplayFunc(&OnDisplay);
	glutTimerFunc(100, OnTimer, 1);
	glutMainLoop();
	return 0;
}

void OnReshape(int w, int h) {
	W = w; H = h;
	width = W; height = H;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < 3; i++) {
		printf("?");
		glGenTextures(1, &ImagesIDs[i]);  //��������
		glBindTexture(GL_TEXTURE_2D, ImagesIDs[i]);   //������

		//vs2019�±������ת��
		WCHAR wfilename[256];
		memset(wfilename, 0, sizeof(wfilename));
		//�ú���ӳ��һ���ַ�����һ�����ַ���unicode�����ַ���
		MultiByteToWideChar(CP_ACP, 0, szFiles[i], strlen(szFiles[i]) + 1, wfilename, sizeof(wfilename) / sizeof(wfilename[0]));
		Images[i] = auxDIBImageLoad(wfilename);  //����ͼƬ
		iWidth = Images[i]->sizeX;
		iHeight = Images[i]->sizeY;
		pImg = Images[i]->data;
		//װ������
		glTexImage2D(GL_TEXTURE_2D, 0, 3, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImg);

		//������ˡ�����ü�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		

		//������
		glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//��������
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
	}
	glEnable(GL_LIGHTING);                          // ��������Ч��
	glEnable(GL_LIGHT0);
}

void OnTimer(int value) {
	day += angle;
	glutPostRedisplay();
	glutTimerFunc(100, OnTimer, 1);
}

//�������
void DrawCircle(GLdouble R) {
	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 1000; i++) {
		GLdouble angle1 = i * 2 * PI / 1000;
		glVertex2d(R * cos(angle1), R * sin(angle1));
	}
	glEnd();
	glPopMatrix();
}

//��������
void gltDrawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks) {
	GLfloat drho = (GLfloat)(3.141592653589) / (GLfloat)iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)(3.141592653589) / (GLfloat)iSlices;
	GLfloat ds = 1.0f / (GLfloat)iSlices;
	GLfloat dt = 1.0f / (GLfloat)iStacks;
	GLfloat t = 1.0f;
	GLfloat s = 0.0f;
	GLint i, j;

	for (i = 0; i < iStacks; i++) {
		GLfloat rho = (GLfloat)i * drho;
		GLfloat srho = (GLfloat)(sin(rho));
		GLfloat crho = (GLfloat)(cos(rho));
		GLfloat srhodrho = (GLfloat)(sin(rho + drho));
		GLfloat crhodrho = (GLfloat)(cos(rho + drho));

		glBegin(GL_TRIANGLE_STRIP);
		s = 0.0f;
		for (j = 0; j <= iSlices; j++) {
			GLfloat theta = (j == iSlices) ? 0.0f : j * dtheta;
			GLfloat stheta = (GLfloat)(-sin(theta));
			GLfloat ctheta = (GLfloat)(cos(theta));

			GLfloat x = stheta * srho;
			GLfloat y = ctheta * srho;
			GLfloat z = crho;

			glTexCoord2f(s, t);
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;
			glTexCoord2f(s, t - dt);
			s += ds;
			glNormal3f(x, y, z);
			glVertex3f(x * fRadius, y * fRadius, z * fRadius);
			
		}
		glEnd();
		t -= dt;
	}
	
}

void OnDisplay() {

	glColor3f(1.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	year = day / 365;
	month = day / 30;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)W / (GLfloat)H, 2, 60.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pox, poy, poz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//̫����
	GLfloat sun_mat_ambient[4] = { 1.0,1.0,1.0,0.0 };
	GLfloat sun_mat_diffuse[4] = { 1.0,1.0,0.5,1.0 };
	GLfloat sun_mat_specular[4] = { 1.0,1.0,1.0,1.0 };
	GLfloat sun_mat_shininess[] = { 10.0 };
	GLfloat sun_mat_emission[4] = { 0.1,0.1,0.1,1.0 };
	GLfloat mat_ambient[4] = { 0.2,0.2,0.2,1.0 };
	GLfloat mat_diffuse[4] = { 1.0,1.0,1.0,1.0 };
	GLfloat mat_specular[4] = { 0.5,0.5,0.5,1.0 };
	GLfloat mat_shininess[] = { 5.0 };
	GLfloat light_position[] = { -10.0,10.0,0.0, 1.0 };//��Դλ��
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);     // ������Դ
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sun_mat_ambient);   // �����趨
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sun_mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sun_mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_mat_emission);

	//����̫��
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);            // ������Դ
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sun_mat_ambient);   // �����趨
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sun_mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sun_mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_mat_emission);
	glBindTexture(GL_TEXTURE_2D, ImagesIDs[0]);
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);  //̫����ת
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gltDrawSphere(1.0, 50, 50);
	glPopMatrix();

	//���Ƶ���ϵͳ
	glPushMatrix();
	//���Ƶ���
	DrawCircle(4.5);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);   // �����趨
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glRotatef((GLfloat)year, 0.0, 1.0, 0.0);  //����ת
	glTranslatef(4.5, 0.0, 0.0);  //��x������
	glRotatef(30.0, -1.0, 0.0, 0.0);//�õ�����б
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, ImagesIDs[2]);
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gltDrawSphere(0.4, 20, 20);
	glPopMatrix();

	//��������
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);   // �����趨
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	DrawCircle(1.0);
	glBindTexture(GL_TEXTURE_2D, ImagesIDs[1]);
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0); //�����Ƶ���ת
	glTranslatef(1.0, 0.0, 0.0);
	glRotatef((GLfloat)month, 0.0, 1.0, 0.0);
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gltDrawSphere(0.15, 10, 10);
	glPopMatrix();
	glPopMatrix();

	glutSwapBuffers();
}
