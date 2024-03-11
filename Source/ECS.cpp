#include "ECS.h"

ECS* ECS::instance = nullptr; // definition class variable

ECS::~ECS() {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		Entity* ret = it->second;
		if (ret) {
			delete ret;
		}
	}
	entds2.clear();
}

void ECS::addEntity(Entity* e) {
	unsigned int i = 1; // or whatever your smallest admissable key value is
	for (auto it = entds2.cbegin(), end = entds2.cend();
		it != end && i == it->first; ++it, ++i)
	{
	}
	e->setID(i);
	entds2[i] = e;
}

void ECS::updatePhysics() {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->updatePhysics();
	}
}

void ECS::deleteEntity(unsigned int ID) {
	Entity* ret = entds2[ID];
	if (ret) {
		btRigidBody* bd = ret->getBody();
		if (bd) {
			Physics::get().getDynamicsWorld()->removeCollisionObject(bd);
		}
		delete ret;
		entds2.erase(ID);
	}

}

Entity* ECS::getEntity(unsigned int ID) {
	return entds2[ID];
}


// WARNING: advances animations by delta!
void ECS::DrawEntities(float delta) {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->Draw(delta);
	}
}