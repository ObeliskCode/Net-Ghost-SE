#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"

class Physics {
	public:
		Physics();
		~Physics();

		void updateSim(float delta);

		btRigidBody* addShape1();
		btRigidBody* addShape2();
		btRigidBody* addShape3();
		btRigidBody* addShape4();
		btRigidBody* addShape5();

		btDiscreteDynamicsWorld* getDynamicsWorld();
		btAlignedObjectArray<btCollisionShape*> getCollisionShapes();
	private:
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
};

#endif
