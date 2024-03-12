#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "Entity.h"
#include <map>

class Physics {
public:
    // defines an class operation that lets clients access its unique instance.
    static Physics& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new Physics;
        return *instance;
    }
    Physics(const Physics&) = delete; // rule of three
    Physics& operator=(const Physics&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

	void updateSim(float delta);

	btRigidBody* addShape1(unsigned int ID);
	btRigidBody* addShape2(unsigned int ID);
	btRigidBody* addShape3(unsigned int ID);
	btRigidBody* addShape4(unsigned int ID);
	btRigidBody* addShape5(unsigned int ID);
	btRigidBody* addShape6(unsigned int ID);

	std::map<btRigidBody*, unsigned int> m_EntityMap;

	btDiscreteDynamicsWorld* getDynamicsWorld();
	btAlignedObjectArray<btCollisionShape*> getCollisionShapes();

private:
    Physics(); // no public constructor
    ~Physics(); // no public destructor
    static Physics* instance; // declaration class variable

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
};

#endif
