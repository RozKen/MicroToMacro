#ifndef __myGLUT_h__
#define __myGLUT_h__

#include <GL/glut.h>		//Used for OpenGL (GLUT)
#include <NxPhysics.h>	//Used for PhysX

void MyGLInit();
void KeyboardCallback(unsigned char key, int x, int y);
void ArrowKeyCallback(int key, int x, int y);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);
void RenderCallback();
void ReshapeCallback(int width, int height);
void IdleCallback();

struct myDimension3{
	int x;
	int y;
	int z;
};

void myGLBox(double x, double y, double z);
void myGLBox(myDimension3* dimensions);
void myGLCylinder(double radius, double height, int sides);

#endif //__myGLUT_h__