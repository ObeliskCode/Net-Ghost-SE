#include "Physics.h"

Physics* Physics::instance = nullptr; // definition class variable

Physics::Physics() {
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -20, 0));
}

Physics::~Physics() {
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	collisionShapes.clear();
}

void Physics::updateSim(float delta) {
	dynamicsWorld->stepSimulation(delta, 0);
}

btRigidBody* Physics::addUnitBoxStaticBody(unsigned int ID, float halfWidth, float halfHeight, float halfLength, float x, float y, float z) {
	btCollisionShape* colShape = new btBoxShape(btVector3(halfWidth, halfHeight, halfLength));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(0.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(x, y, z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;

}


btRigidBody* Physics::addShape1(unsigned int ID) {
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(60.), btScalar(50.), btScalar(60.)));

	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -45, 0));

	btScalar mass(0.);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		groundShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btRigidBody* Physics::addShape2(unsigned int ID) {
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(4, 20, 0));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	body->setRollingFriction(btScalar(0.2f));

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btRigidBody* Physics::addShape3(unsigned int ID) {
	btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(-4, 23, 0));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	body->setRollingFriction(btScalar(0.2f));

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btRigidBody* Physics::addShape4(unsigned int ID) {
	btCollisionShape* colShape = new btBoxShape(btVector3(1, 1, 1));
	//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(0, 23, 0));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// what does this do?
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setAngularFactor(btScalar(0));

	body->setFriction(btScalar(3.f));

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btRigidBody* Physics::addShape5(unsigned int ID) {
	btCollisionShape* colShape = new btCapsuleShape(btScalar(1.), btScalar(2.));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(1.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(0, 23, -4));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btRigidBody* Physics::addShape6(unsigned int ID) {
	btCollisionShape* colShape = new btCapsuleShapeZ(btScalar(.06f), btScalar(1.25f));
	collisionShapes.push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();
	//float CosHalfPi = sqrt(2.f) / 2.f;
	//startTransform.setRotation(btQuaternion(CosHalfPi, -CosHalfPi, 0.f, 0.f));

	btScalar mass(0.5f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(17, 10, 32));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	body->setRollingFriction(btScalar(0.2f));

	dynamicsWorld->addRigidBody(body);

	m_EntityMap[body] = ID;

	return body;
}

btDiscreteDynamicsWorld* Physics::getDynamicsWorld(){
	return dynamicsWorld;
};

btAlignedObjectArray<btCollisionShape*> Physics::getCollisionShapes() {
	return collisionShapes;
};