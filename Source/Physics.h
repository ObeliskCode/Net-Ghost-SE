#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Entity.h"
#include <map>

class Physics {
	public:
		Physics();
		~Physics();

		void updateSim(float delta);

		btRigidBody* addShape1(unsigned int ID);
		btRigidBody* addShape2(unsigned int ID);
		btRigidBody* addShape3(unsigned int ID);
		btRigidBody* addShape4(unsigned int ID);
		btRigidBody* addShape5(unsigned int ID);

		std::map<btRigidBody*, unsigned int> m_EntityMap;

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
