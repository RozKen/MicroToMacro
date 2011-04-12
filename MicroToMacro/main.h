	/*
	 *	Prototype Declaration
	 */
void InitGLUT(int argc, char ** argv);
bool InitNx();
void CleanUpNx();
bool InitScene();
void CreateGroundPlane();
NxActor* CreateBox(float w, float d, float h,float xInit, float yInit, float zInit);
NxActor* CreateBox(NxVec3 dimensions, NxVec3 pos);
NxActor* CreateSphere(float r, float xInit, float yInit, float zInit);
NxActor* CreateSphere(float r, NxVec3 pos);
void CreateTumblingRobot(const NxVec3 pos);