#pragma once
#include <btBulletDynamicsCommon.h>

#pragma comment(lib, "BulletCollision_Debug.lib")
#pragma comment(lib, "BulletDynamics_Debug.lib")
#pragma comment(lib, "BulletFileLoader_Debug.lib")
#pragma comment(lib, "BulletSoftBody_Debug.lib")
#pragma comment(lib, "BulletWorldImporter_Debug.lib")
#pragma comment(lib, "BulletXmlWorldImporter_Debug.lib")
#pragma comment(lib, "ConvexDecomposition_Debug.lib")
#pragma comment(lib, "GIMPACTUtils_Debug.lib")
#pragma comment(lib, "HACD_Debug.lib")
#pragma comment(lib, "LinearMath_Debug.lib")

#include "BasicElement.h"
#include <vector>
using namespace std;

enum SimpleElementType {
	CUBE,
	SPHERE,
	CONE,
	CIRCLE,
	CYLINDER,
	RECTANGLE
};

struct RigidBody2Element {
	bool enable;
	btRigidBody* rigidbody;
	BasicElement*element;

	RigidBody2Element(btRigidBody*_rb, BasicElement* _be, bool _enable=true)
		:rigidbody(_rb), element(_be),enable(_enable)  {}
};

class Physics {
private:
	static btBroadphaseInterface* broadphase;
	static btDefaultCollisionConfiguration* collisionConfiguration;
	static btCollisionDispatcher* dispatcher;
	static btSequentialImpulseConstraintSolver* solver;
	static btCollisionShape* groundShape;
	static btDefaultMotionState* groundMotionState;
	static btRigidBody* groundRigidBody;
	static vector<RigidBody2Element> rigidbody2elements;
	static btDiscreteDynamicsWorld* dynamicsWorld;
public:
	static void init();
	static void free();
	static void update();
	static void AddRigidBodyAndElement(btRigidBody* rigidbody, BasicElement* element);
	static btRigidBody * CreateSimpleRigidBody(const BasicElement * element, SimpleElementType type, double mass = 1, Vec InertiaVec= Vec(0, 0, 0));
	Physics() {};
	~Physics() {};
};