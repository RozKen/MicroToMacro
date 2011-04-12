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
#include "main.h"

#pragma comment(lib, "PhysXLoader.lib")
//↑"プロジェクトのプロパティ>構成プロパティ>リンカ>入力>追加の依存ファイル>PhysXLoader.lib"の設定がしてないときに必要.

	/*
	 *	Global Variables
	 */
NxPhysicsSDK* pPhysicsSDK = NULL;	//PhysX
NxScene* pScene = NULL;					//Scene
NxVec3 DefaultGravity(0,-98,0);			//Gravity
bool isSimulate = false;						//Flag for Simulation
std::vector<TumblingRobots> robots;

using namespace std;							//std名前空間を利用

	/**
	 * main
	 * @description - Entry Point for this Program
	 */
void main(int argc, char ** argv){
	InitGLUT(argc, argv);
	InitNx();
	InitScene();
	CreateGroundPlane();
	//CreateBox(10.0f, 10.0f, 10.0f, 10.0f, 60.0f, 10.0f);	//20x20x20の箱を(10,60,10)へ
	//CreateSphere(10.0f , -50.0f, 60.0f, 10.0f);					//半径10の球を(-20, 60, 10)へ
	//CreateTumblingRobot(NxVec3(-20.0f, 50.0f, 0.0f));	//二腕ロボットを(-20, 50, 0)へ
	int delta = 60;
	int boundary = 500;
	for(int i = -boundary; i < boundary + 1; i += delta){
		for(int j = -boundary; j < boundary+1; j += delta){
			CreateTumblingRobot(NxVec3(i, 50, j));
		}
	}

	cout << "===Viewport Navigation (Maya Style)===" <<endl;
	cout << "Rotate: Right Drag" << endl;
	cout << "Move: Cener Drag" << endl;
	cout << "Zoom: Right and Center Drag" << endl;
	cout << "===Simulation Navigation===" << endl;
	cout << "Simulation ON/OFF: Space Key" << endl;
	cout << "===Object Creation===" << endl;
	cout << "s: sphere, b: box, t: tumbling robot" << endl;
	cout << "===Robot Manipulation===" << endl;
	cout << "1: left arm, 2: right arm, 3: both arms" << endl;

	cout << "MAX_Real" << NX_MAX_REAL <<endl;

	/*
	 *	Render the Scene for each Simulate Time Steps
	 */
	glutMainLoop();
}

	/**
	 * CreateTumblingRobot(NxVec3 pos);
	 * @NxVec3 pos : Position Vector of this Robot
	 * @return
	 */
void CreateTumblingRobot(const NxVec3 pos){
	if(pScene == NULL) return;
	//NxActor* body = CreateSphere(15.0f, pos);
	NxActor* body = CreateBox(NxVec3(15.0, 15.0, 15.0), pos);
	body->setMass(5);
	//body->setMass(15);
	NxVec3 armDim = NxVec3(5, 30, 1.5);
	NxVec3 leftArmLocalPos = NxVec3(-20, 0, 0);
	NxVec3 rightArmLocalPos = NxVec3(20, 0, 0);
	NxActor* leftArm = CreateBox(armDim, pos + leftArmLocalPos);
	NxActor* rightArm = CreateBox(armDim, pos + rightArmLocalPos);
	leftArm->setMass(1);
	rightArm->setMass(1);
	//Describe Revolute Joint
	NxRevoluteJointDesc revoluteDesc1;
	revoluteDesc1.setToDefault();
	revoluteDesc1.actor[0] = body;
	revoluteDesc1.actor[1] = leftArm;
	revoluteDesc1.setGlobalAnchor(pos + NxVec3(-10, 0, 0));
	revoluteDesc1.setGlobalAxis(NxVec3(1, 0, 0));
	NxJoint *joint1 = pScene->createJoint(revoluteDesc1);

	NxRevoluteJointDesc revoluteDesc2;
	revoluteDesc2.setToDefault();
	revoluteDesc2.actor[0] = body;
	revoluteDesc2.actor[1] = rightArm;
	revoluteDesc2.setGlobalAnchor(pos + NxVec3(10, 0, 0));
	revoluteDesc2.setGlobalAxis(NxVec3( 1, 0, 0 ) );
	NxJoint *join2 = pScene->createJoint(revoluteDesc2);
	
	TumblingRobots robot;
	robot.body = body;
	robot.leftArm = leftArm;
	robot.rightArm = rightArm;
	robots.push_back(robot);
	return;
}
	/**
	 * CreateSphere(float r, float xInit, float yInit, float zInit)
	 * @float r: radius of the sphere
	 * @float xInit: Initial Position on X
	 * @float yInit: Initial Position on Y
	 * @float zInit: Initial Position on Z
	 * @return pActor: Pointer to Created Actor
	 */
NxActor* CreateSphere(float r, float xInit, float yInit, float zInit){
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxSphereShapeDesc sphereDesc;

	//デフォルト値で初期化 (クリア)
	actorDesc.setToDefault();
	bodyDesc.setToDefault();
	sphereDesc.setToDefault();
	//bodyDesc.angularDamping = 0.5f;	//転がり摩擦係数

	sphereDesc.radius = r;									//球の半径を指定
	sphereDesc.userData = (void *)size_t(sphereDesc.radius);
	actorDesc.shapes.pushBack(&sphereDesc);	//球をアクターに追加
	actorDesc.body = &bodyDesc;	//動的情報を指定
	actorDesc.density = 30000;//7874.0f;		//密度7874 kg/m^3 : 鉄の密度
	actorDesc.globalPose.t = NxVec3(xInit, yInit, zInit);	//Scene上の位置
	//Set userData to NULL if you are not doing anyting with it.
	NxActor*pActor = pScene->createActor( actorDesc );
	return pActor;
}

NxActor* CreateSphere(float r, NxVec3 pos){
	return CreateSphere(r, pos.x, pos.y, pos.z);
}

	/**
	 * CreateBox(float w, float d, float h,float xInit, float yInit, float zInit)
	 * @float w: width of the box
	 * @float d: depth of the box
	 * @float h: height of the box
	 * @float xInit: Initial Position on X
	 * @float yInit: Initial Position on Y
	 * @float zInit: Initial Position on Z
	 * @return pActor: Pointer to Created Actor
	 */
NxActor* CreateBox(float w, float d, float h,float xInit, float yInit, float zInit){
	/*
	 *	Create a Box Actor
	 * Dynamic Actor: Rigid Bodies
	 */
	//Create a Body Descriptor
	NxBodyDesc bodyDesc;					//Box用 Body Descriptor
	
	//The Actor Descriptor
	NxActorDesc actorDesc;
	
	//Box Shape Descriptor
	NxBoxShapeDesc boxDesc;

	//デフォルト値で初期化 (クリア)
	actorDesc.setToDefault();
	bodyDesc.setToDefault();
	boxDesc.setToDefault();

	//bodyDesc.angularDamping = 0.5f;	//回転減衰係数????
	//bodyDesc.linearVelocity = NxVec3 (-30, -10, -10); //初期速度はX軸方向に1
	bodyDesc.linearDamping = 0.1f;

	actorDesc.body = &bodyDesc;

	boxDesc.dimensions = NxVec3( w, d, h );	//20.0 x 20.0 x 20.0の直方体
	myDimension3* myD = new myDimension3;
	myD->x = boxDesc.dimensions.x;
	myD->y = boxDesc.dimensions.y;
	myD->z = boxDesc.dimensions.z;
	//boxDesc.userData = (void *)(boxDesc.dimensions);	//wの2倍を一辺の長さとして表示
	boxDesc.userData = (void *)myD;
	actorDesc.shapes.pushBack( &boxDesc );	//ActorにBodyを登録
	actorDesc.density = 100;//7874.0f;	//密度7874 kg/m^3 : 鉄の密度
	actorDesc.globalPose.t = NxVec3( xInit, yInit, zInit);		//初期位置(10.0, 10.0, 10.0)
	
	//Set userData to NULL if you are not doing anyting with it.
	NxActor* pActor = pScene->createActor( actorDesc );
	return pActor;
}

NxActor* CreateBox(NxVec3 dimensions, NxVec3 pos){
	return CreateBox(dimensions.x, dimensions.y, dimensions.z,
		pos.x, pos.y, pos.z);
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
	//平面方程式: ax + by + cz + d = 0;
	planeDesc.normal = NxVec3(0, 1, 0);		//面の法線はY軸(↑)方向
	planeDesc.d = 0.0f;								//Y = 0.0fに面を作る

	actorDesc.shapes.pushBack( &planeDesc );	//ActorにPlaneを登録
	
	//NxScene Creates Actor and Returns a Pointer.
	NxActor* pActor = pScene->createActor( actorDesc);
	pActor->userData = NULL;		//PhysX上のActorと(ゲームなどの)データ上のActorを結びつける
	
	//Set the parameters for the Default Material
	//Physicsの"Material"とは目に見える表面材質ではなく，物体の物理学的特徴を表す
	NxMaterial* defaultMaterial = pScene->getMaterialFromIndex( 0 );
	defaultMaterial->setRestitution( 0.3f );			//反発係数
	defaultMaterial->setStaticFriction( 0.5f );		//静止摩擦係数
	defaultMaterial->setDynamicFriction( 0.5f );	//動摩擦係数
}
	/**
	 * InitGLUT(int argc, char ** argv) : Initialize GLUT
	 * @variable int argc : メイン関数のコマンドライン入力の数
	 * @variable int argv : メイン関数のコマンドライン入力の内容
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
