/*************************************************************************
 *	glutCallBacks.cpp
 * @author - Kenichi Yorozu
 * @project_description - Emergence from Chaos - Micro to Macro DEMO
 * @description - OpenGL Handling Code
 ***********************************************************************/

#include <iostream>
#include "myGLUT.h"		//Prototype Declarations
#include "main.h"
//glut.hを後ろに置かないと，exit関数についてエラーが生じる.
//#pragma comment(lib, "glut32.lib")
// Rendering
NxVec3	gEye(50.0f, 50.0f, 50.0f);	//Cameraの視点
NxVec3	gDir(-1.0f,-1.0f,-1.0f);			//Cameraの向く方向
NxVec3	gViewY;								//
int		gMouseX = 0;							//マウスポインタの位置X
int		gMouseY = 0;							//マウスポインタの位置Y
unsigned char gMouseButton[3] = {0};	//マウスのボタンの押下状態

extern NxScene* pScene;						//Scene
extern bool isSimulate;								//Flag for Simulation

void MyGLInit(){
	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float ambientColor[]	= { 0.1f, 0.1f, 0.1f, 0.0f };
	float diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	float specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	float position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	
	glClearColor(0.5, 0.5, 0.5, 1.0);		//背景色を設定
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	int n = robots.size();
	switch(key)
	{
		case 27:			//ESCキーが押されたら
			exit(0);			//プログラムを終了する
			break;
		case 32:			//Spaceキーが押されたら
			isSimulate = !isSimulate;		//Simulate状態をON/OFFする
			break;
		case 's':
			CreateSphere(2.5f, 0, 3, 2.5f);
			break;
		case 'b':
			CreateBox(5,5,5, 2.5f, 2.5f, -10);
			break;
		case 't':
			CreateTumblingRobot(NxVec3(-20, 30, -20));
				break;
		//////////Parameter調整中///////////////////
		case '1':
			for(int i = 0; i < n; i++){
				robots[i].leftArm->addLocalTorque(NxVec3(50000000, 0, 0));
				//robots[i].leftArm->addLocalTorque(NxVec3(50000000000, 0, 0));
				//robots[i].leftArm->addLocalForce(NxVec3(0, 0, 5000000));
			}
			break;
		case '2':
			for(int i = 0; i < n; i++){
				robots[i].rightArm->addLocalTorque(NxVec3(5000000, 0, 0));
				//robots[i].rightArm->addLocalTorque(NxVec3(50000000000, 0, 0));
			}
			break;
		case '3':
			for(int i = 0; i < n; i++){
				robots[i].leftArm->addLocalTorque(NxVec3(500000, 0, 0));
				robots[i].rightArm->addLocalTorque(NxVec3(500000, 0, 0));
				//robots[i].leftArm->addLocalTorque(NxVec3(50000000000, 0, 0));
				//robots[i].rightArm->addLocalTorque(NxVec3(50000000000, 0, 0));
				//robots[i].leftArm->addLocalForce(NxVec3(0, 0, 5000000));
			}
			break;
	}
}

void ArrowKeyCallback(int key, int x, int y)
{
	KeyboardCallback(key,x,y);
}

void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;
	switch(button){
		case GLUT_LEFT_BUTTON:
			gMouseButton[0] = ((GLUT_DOWN==state)?1:0);
			break;
		case GLUT_MIDDLE_BUTTON:
			gMouseButton[1] = ((GLUT_DOWN==state)?1:0);
			break;
		case GLUT_RIGHT_BUTTON:
			gMouseButton[2] = ((GLUT_DOWN==state)?1:0);
			break;
		default:
			break;
	}
	glutPostRedisplay();
}

void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;
	
	gDir.normalize();		//カメラの視線ベクトルを正規化
	gViewY.cross(gDir, NxVec3(0,1,0));	//

	if( gMouseButton[0] && gMouseButton[1] ){
		//Zoom: Left + Center Buttons Drag
		gEye -= gDir * 0.5f * dy;
	}else{
		if( gMouseButton[0] ){
			//Rotate: Left Button Drag
			NxQuat qx(NxPiF32 * dx * 10/ 180.0f, NxVec3(0,1,0));
			qx.rotate(gDir);
			NxQuat qy(NxPiF32 * dy * 10/ 180.0f, gViewY);
			qy.rotate(gDir);
		}else if( gMouseButton[1] ){
			//Move: Center Button Drag
			gEye += 0.1f * (gViewY * dx - NxVec3(0, 1, 0) * dy);
		}
	}
	gMouseX = x;
	gMouseY = y;
	glutPostRedisplay();
}

void RenderCallback()
{
	if(pScene == NULL){
#ifdef _DEBUG
		std::cout << "pScene == NULL" << std::endl;
#endif //_DEBUG
		return;
	}
	
	// Start simulation
	if (isSimulate){
		pScene->simulate(1.0f/60.0f);
	}
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render all actors
	int nbActors = pScene->getNbActors();
	NxActor** actors = pScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;
		if(actor->userData == NULL){
			// draw grid
			glBegin(GL_LINES);
				int y = 0;
				for(int i=-100;i<=100;++i) {
					glVertex3f(i*10,y,-1000);
					glVertex3f(i*10,y,1000);

					glVertex3f(1000,y,i*10);
					glVertex3f(-1000,y,i*10);
				}
			glEnd();
		}
		int nbShapes = actor->getNbShapes();
		NxShape* const* shapes = actor->getShapes();
		while(nbShapes--){
			NxShape* shape = shapes[nbShapes];
			switch(shape->getType()){
				float glMat[16];
				float glMat2[16];
				case NX_SHAPE_BOX:
					// Render actor
					glPushMatrix();
					{				
						actor->getGlobalPose().getColumnMajor44(glMat);
						glMultMatrixf(glMat);
						shape->getLocalPose().getColumnMajor44(glMat2);
						glMultMatrixf(glMat2);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						myGLBox((myDimension3 *)shape->userData);
					}
					glPopMatrix();

					// Render shadow
					glPushMatrix();
					{
						const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
						glMultMatrixf(shadowMat);
						glMultMatrixf(glMat);
						glMultMatrixf(glMat2);
						glDisable(GL_LIGHTING);
						glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
						myGLBox((myDimension3 *)shape->userData);
						glEnable(GL_LIGHTING);
					}
					glPopMatrix();
					break;
				case NX_SHAPE_SPHERE:
					// Render actor
					glPushMatrix();
					{
						actor->getGlobalPose().getColumnMajor44(glMat);
						glMultMatrixf(glMat);
						shape->getLocalPose().getColumnMajor44(glMat2);
						glMultMatrixf(glMat2);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glutSolidSphere(size_t(shape->userData),16, 16);
					}
					glPopMatrix();

					// Render shadow
					glPushMatrix();
					{
						const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
						glMultMatrixf(shadowMat);
						glMultMatrixf(glMat);
						glMultMatrixf(glMat2);
						glDisable(GL_LIGHTING);
						glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
						glutSolidSphere(size_t(shape->userData),16, 16);
						glEnable(GL_LIGHTING);
					}
					glPopMatrix();
					break;
				default:
					break;
			}
		}
	}

	// Fetch simulation results
	if(isSimulate){
		pScene->flushStream();
		pScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}

	glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

void IdleCallback()
{
	glutPostRedisplay();
}

/**
 * My Primitive Shapes for Open GL Presentation
 */

/**
 * myGLBox : Draw Box (not only cube)
 * @double x : Box Size (x-Axis)
 * @double y : Box Size (y-Axis)
 * @double z : Box Size (z-Axis)
 * @return void
 */
void myGLBox(double x, double y, double z){
	GLdouble vertex[][3] = {
		{ -x, -y, -z },
		{  x, -y, -z },
		{  x,  y, -z },
		{ -x,  y, -z },
		{ -x, -y,  z },
		{  x, -y,  z },
		{  x,  y,  z },
		{ -x,  y,  z }
	};

	const static int face[][4] = {
		{ 0, 1, 2, 3 },
		{ 1, 5, 6, 2 },
		{ 5, 4, 7, 6 },
		{ 4, 0, 3, 7 },
		{ 4, 5, 1, 0 },
		{ 3, 2, 6, 7 }
	};

	const static GLdouble normal[][3] = {
		{ 0.0, 0.0,-1.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{-1.0, 0.0, 0.0 },
		{ 0.0,-1.0, 0.0 },
		{ 0.0, 1.0, 0.0 }
	};
	int i, j;
	glBegin(GL_QUADS);
		for (j = 0; j < 6; ++j) {
			glNormal3dv(normal[j]);
			for (i = 4; --i >= 0;) {
				glVertex3dv(vertex[face[j][i]]);
			}
		}
	glEnd();
}
void myGLBox(myDimension3* dimensions){
	myGLBox((double)dimensions->x, (double)dimensions->y, (double)dimensions->z);
	return;
}
/**
 * myGLCylinder : Draw Cylinder
 * @double radius : Cylinder Radius
 * @double height : Cylinder Height
 * @double sides : Number of Divisions of Sides
 * @return void
 */
void myGLCylinder(double radius, double height, int sides){
	double step = 6.28318530717958647692 / (double)sides;
	int i = 0;
	/* 上面 */
	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
		while (i < sides) {
			double t = step * (double)i++;
			glVertex3d(radius * sin(t), height, radius * cos(t));
		}
	glEnd();

	/* 底面 */
	glNormal3d(0.0, -1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
		while (--i >= 0) {
			double t = step * (double)i;
			glVertex3d(radius * sin(t), -height, radius * cos(t));
		}
	glEnd();

	/* 側面 */
	glBegin(GL_QUAD_STRIP);
		while (i <= sides) {
			double t = step * (double)i++;
			double x = sin(t);
			double z = cos(t);

			glNormal3d(x, 0.0, z);
			glVertex3f(radius * x, height, radius * z);
			glVertex3f(radius * x, -height, radius * z);
		}
	glEnd();
}