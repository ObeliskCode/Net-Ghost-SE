#include "EntityComponentSystem.h"

void EntityComponentSystem::addEntity(Entity* e) {
	unsigned int i = 1; // or whatever your smallest admissable key value is
	for (auto it = entds2.cbegin(), end = entds2.cend();
		it != end && i == it->first; ++it, ++i)
	{
	}
	e->setID(i);
	entds2[i] = e;
}

void EntityComponentSystem::updatePhysics() {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->updatePhysics();
	}
}

void EntityComponentSystem::removeEntity(unsigned int ID) {
	delete entds2[ID];
	entds2.erase(ID);
}

Entity* EntityComponentSystem::getEntity(unsigned int ID) {
	return entds2[ID];
}


// WARNING: advances animations by delta!
void EntityComponentSystem::DrawEntities(float delta) {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->Draw(delta);
	}
}