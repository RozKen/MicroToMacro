/*************************************************************************
 *	introPhysX.cpp
 * @author - Kenichi Yorozu
 * @description - Simple Sample Code of PhysX
 ***********************************************************************/

#include <iostream>		//Used for Console Message I/O
#include <string>			//Used for Console Window Closing Stopper
#include <NxPhysics.h>	//Used for PhysX
#include <GL/glut.h>		//Used for OpenGL (GLUT)
#include "myGLUT.h"	//Import Callback Functions for GLUT
#pragma comment(lib, "PhysXLoader.lib")
//��"�v���W�F�N�g�̃v���p�e�B>�\���v���p�e�B>�����J>����>�ǉ��̈ˑ��t�@�C��>PhysXLoader.lib"�̐ݒ肪���ĂȂ��Ƃ��ɕK�v.
	/*
	 *	Prototype Declaration
	 */
void InitGLUT(int argc, char ** argv);
bool InitNx();
void CleanUpNx();
bool InitScene();
void CreateGroundPlane();
void CreateBox(float w, float d, float h,float xInit, float yInit, float zInit);
void CreateSphere(float r, float xInit, float yInit, float zInit);

	/*
	 *	Global Variables
	 */
NxPhysicsSDK* pPhysicsSDK = NULL;	//PhysX
NxScene* pScene = NULL;					//Scene
NxVec3 DefaultGravity(0,-9.8,0);			//Gravity
bool isSimulate = false;						//Flag for Simulation

using namespace std;							//std���O��Ԃ𗘗p

	/**
	 * main
	 * @description - Entry Point for this Program
	 */
void main(int argc, char ** argv){
	InitGLUT(argc, argv);
	InitNx();
	InitScene();
	CreateGroundPlane();
	CreateBox(10.0f, 10.0f, 10.0f, 10.0f, 60.0f, 10.0f);
	CreateSphere(10.0f , -20.0f, 60.0f, 10.0f);

	cout << "===Viewport Navigation (Maya Style)===" <<endl;
	cout << "Rotate: Right Drag" << endl;
	cout << "Move: Cener Drag" << endl;
	cout << "Zoom: Right and Center Drag" << endl;
	cout << "===Simulation Navigation===" << endl;
	cout << "Simulation ON/OFF: Space Key" << endl;

	/*
	 *	Render the Scene for each Simulate Time Steps
	 */
	glutMainLoop();
}
	/**
	 * CreateSphere(float r, float xInit, float yInit, float zInit)
	 * @float r: radius of the sphere
	 * @float xInit: Initial Position on X
	 * @float yInit: Initial Position on Y
	 * @float zInit: Initial Position on Z
	 * @return
	 */
void CreateSphere(float r, float xInit, float yInit, float zInit){
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxSphereShapeDesc sphereDesc;

	//�f�t�H���g�l�ŏ����� (�N���A)
	actorDesc.setToDefault();
	bodyDesc.setToDefault();
	sphereDesc.setToDefault();

	bodyDesc.angularDamping = 0.5f;	//�]���薀�C�W��

	sphereDesc.radius = r;									//���̔��a���w��
	actorDesc.shapes.pushBack(&sphereDesc);	//�����A�N�^�[�ɒǉ�
	actorDesc.body = &bodyDesc;	//���I�����w��
	actorDesc.density = 100.0f;			//���ʖ��x (kg/m^3)
	actorDesc.globalPose.t = NxVec3(xInit, yInit, zInit);	//Scene��̈ʒu
	//Set userData to NULL if you are not doing anyting with it.
	NxActor*pActor = pScene->createActor( actorDesc );
	pActor->userData = (void *)size_t((int)r);
}
	/**
	 * CreateBox(float w, float d, float h,float xInit, float yInit, float zInit)
	 * @float w: width of the box
	 * @float d: depth of the box
	 * @float h: height of the box
	 * @float xInit: Initial Position on X
	 * @float yInit: Initial Position on Y
	 * @float zInit: Initial Position on Z
	 * @return
	 */
void CreateBox(float w, float d, float h,float xInit, float yInit, float zInit){
	/*
	 *	Create a Box Actor
	 * Dynamic Actor: Rigid Bodies
	 */
	//Create a Body Descriptor
	NxBodyDesc bodyDesc;					//Box�p Body Descriptor
	bodyDesc.angularDamping = 0.5f;	//��]�����W��????
	bodyDesc.linearVelocity = NxVec3 (-30, -10, -10); //�������x��X��������1
	
	//The Actor Descriptor
	NxActorDesc actorDesc;
	actorDesc.body = &bodyDesc;
	
	//Box Shape Descriptor
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3( w, d, h );	//20.0 x 20.0 x 20.0�̒�����
	actorDesc.shapes.pushBack( &boxDesc );	//Actor��Body��o�^
	actorDesc.density = 100.0f;	//���x10.0
	actorDesc.globalPose.t = NxVec3( xInit, yInit, zInit);		//�����ʒu(10.0, 10.0, 10.0)
	
	//Set userData to NULL if you are not doing anyting with it.
	NxActor*pActor = pScene->createActor( actorDesc );
	pActor->userData = (void *)size_t((int)w*2.0);
}
	/**
	 * CreateGroundPlane(): Create Ground Plane
	 * @return
	 */
void CreateGroundPlane(){
	/*
	 *	Create Ground Plane
	 * Static Actor: has no 'NxBodyDesc'
	 */
	//actor Descriptor with Collection of Shapes.
	NxActorDesc	actorDesc;

	//Plane Shape Descriptor
	NxPlaneShapeDesc	planeDesc;
	//���ʕ�����: ax + by + cz + d = 0;
	planeDesc.normal = NxVec3(0, 1, 0);		//�ʂ̖@����Y��(��)����
	planeDesc.d = 0.0f;								//Y = 0.0f�ɖʂ����

	actorDesc.shapes.pushBack( &planeDesc );	//Actor��Plane��o�^
	
	//NxScene Creates Actor and Returns a Pointer.
	NxActor* pActor = pScene->createActor( actorDesc);
	pActor->userData = NULL;		//PhysX���Actor��(�Q�[���Ȃǂ�)�f�[�^���Actor�����т���
	
	//Set the parameters for the Default Material
	//Physics��"Material"�Ƃ͖ڂɌ�����\�ʍގ��ł͂Ȃ��C���̂̕����w�I������\��
	NxMaterial* defaultMaterial = pScene->getMaterialFromIndex( 0 );
	defaultMaterial->setRestitution( 0.3f );			//�����W��
	defaultMaterial->setStaticFriction( 0.5f );		//�Î~���C�W��
	defaultMaterial->setDynamicFriction( 0.5f );	//�����C�W��
}
	/**
	 * InitGLUT(int argc, char ** argv) : Initialize GLUT
	 * @variable int argc : ���C���֐��̃R�}���h���C�����͂̐�
	 * @variable int argv : ���C���֐��̃R�}���h���C�����͂̓��e
	 * @return
	 */
void InitGLUT(int argc, char ** argv){
	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow("Simple Sample Code of Nvidia PhysX");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	MyGLInit();
	atexit(CleanUpNx);
	return;
}
	/**
	 *	InitNx() : Initialize PhysX
	 *	@return bool - whether Physics SDK Initialization has done or not.
	 */
bool InitNx()
{
	bool initialized = false;
#ifdef _DEBUG
	std::cout << "Physics SDK Loading Version: " << NX_PHYSICS_SDK_VERSION << std::endl;
#endif //_DEBUG
	pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (pPhysicsSDK != NULL){
		pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
#ifdef _DEBUG
		std::cout << "PhysX Initialized" << std::endl;
#endif //_DEBUG
		initialized = true;
	}else{
		std::cout<<"Initialize Error: Cannot Create Physics SDK"<<std::endl;
	}
	return initialized;
}
	/**
	 *	InitScene() : Initialize Scene
	 * @return bool - whether Scene Initialization has done or not.
	 * @description - Initialize Scene.
	 */
bool InitScene(){
	bool sceneInit = false;
	//Create Scene Descripter
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = DefaultGravity;
	//Create Real Scene
	pScene = pPhysicsSDK->createScene(sceneDesc);
	if (pScene != NULL){
#ifdef _DEBUG
		std::cout << "Scene Initialized" << std::endl;
#endif //_DEBUG
		sceneInit = true;
	}
	return sceneInit;
}
	/**
	 *	CleanUpNx() : Clean Up PhysX Objects
	 *	@description - Clean Up Memory.
	 */
void CleanUpNx(){
	if(pPhysicsSDK != NULL){
		if( pScene != NULL ){
			pPhysicsSDK->releaseScene(*pScene);
			pScene = NULL;	//Release Scene Object
		}
		NxReleasePhysicsSDK( pPhysicsSDK );
		pPhysicsSDK = NULL;	//Release PhysicsSDK Object
	}
#ifdef _DEBUG
		std::cout << "Clean Up Ended." << std::endl;
#endif //_DEBUG
}
