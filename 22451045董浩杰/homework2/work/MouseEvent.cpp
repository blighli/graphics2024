#include "MouseEvent.h"
#include <stdio.h>

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };
const float MINSCALE = { 0.05f };

float	Scale;					// scaling factor
char* selectedObject = "None";
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

int		ActiveButton;			// current button that is down
int		Xmouse, Ymouse;			// mouse values

float	Xrot, Yrot;				// rotation angles in degrees
GLfloat sunX = 0.0f, sunY = 0.0f, sunZ = 0.0f, sunRadius = 3.0f;
GLfloat earthX = 3.0f, earthY = 0.0f, earthZ = 0.0f, earthRadius = 0.4f;
GLfloat moonX = 3.6f, moonY = 0.0f, moonZ = 0.0f, moonRadius = 0.07f;



void project3DTo2D(GLfloat x, GLfloat y, GLfloat z, GLint viewport[4], GLdouble modelview[16], GLdouble projection[16], GLfloat& screenX, GLfloat& screenY) {
	GLdouble winX, winY, winZ;
	gluProject(x, y, z, modelview, projection, viewport, &winX, &winY, &winZ);
	screenX = (GLfloat)winX;
	screenY = (GLfloat)winY;
}

bool isPointInSphere(int mouseX, int mouseY, float sphereX, float sphereY, float sphereZ, float sphereRadius, int windowWidth, int windowHeight)
{
	// ����������ת��Ϊ OpenGL �Ĺ�һ���豸���� (NDC)
	float ndcX = (2.0f * mouseX) / windowWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / windowHeight;

	// ��ȡ��ǰ��ģ����ͼ�����ͶӰ����
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// �� NDC ����ת��Ϊ��������
	float depth;
	double worldX, worldY, worldZ;
	gluUnProject(ndcX, ndcY, 0.0, modelview, projection, viewport, &worldX, &worldY, &worldZ);

	// ������λ�����������ĵľ���
	float dx = worldX - sphereX;
	float dy = worldY - sphereY;
	float dz = 0;
	float distance = sqrt(dx * dx + dy * dy + dz * dz);

	// �жϾ����Ƿ�С������İ뾶
	return distance <= sphereRadius;
}

void MouseButton(int button, int state, int x, int y)
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	// get the proper button bit mask:
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		b = LEFT;		break;
	case GLUT_MIDDLE_BUTTON:
		b = MIDDLE;		break;
	case GLUT_RIGHT_BUTTON:
		b = RIGHT;		break;
	default:
		b = 0;
		fprintf(stderr, "Unknown mouse button: %d\n", button);
	}
	// button down sets the bit, up clears the bit:
	if (state == GLUT_DOWN)
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b; // ����λ����

		// ����Ƿ�����̫��
		if (isPointInSphere(x, y, sunX, sunY, sunZ, sunRadius, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)))
		{
			selectedObject = "Sun";
		}
		// ����Ƿ����˵���
		else if (isPointInSphere(x, y, earthX, earthY, earthZ, earthRadius, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)))
		{
			selectedObject = "Earth";
		}
		// ����Ƿ���������
		else if (isPointInSphere(x, y, moonX, moonY, moonZ, moonRadius, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)))
		{
			selectedObject = "Moon";
		}
		else
		{
			selectedObject = "None";
		}
	}
	
}

// called when the mouse moves while a button is down:
void MouseMotion(int x, int y)
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;
	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}
	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);
		// keep object from turning inside-out or disappearing:
		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}
	Xmouse = x;			// new current position
	Ymouse = y;
	//glutSetWindow(MainWindow);
	glutPostRedisplay();
}
// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
void Reset()
{
	ActiveButton = 0;
	//AxesOn = 1;
	//DebugOn = 0;
	//DepthBufferOn = 1;
	//DepthFightingOn = 0;
	//DepthCueOn = 0;
	Scale = 1.0;
	//WhichColor = WHITE;
	//WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}