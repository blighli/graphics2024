#include "MouseEvent.h"
#include <stdio.h>

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };
const float MINSCALE = { 0.05f };

float	Scale;					// scaling factor

const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

int		ActiveButton;			// current button that is down
int		Xmouse, Ymouse;			// mouse values

float	Xrot, Yrot;				// rotation angles in degrees

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
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
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