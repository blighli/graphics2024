#pragma once

#include <gl/glut.h>
#include <math.h>
extern float	Scale;					// scaling factor
extern float	Xrot, Yrot;				// rotation angles in degrees

void MouseMotion(int x, int y);
void MouseButton(int button, int state, int x, int y);
void Reset();