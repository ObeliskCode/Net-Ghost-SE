#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include "Entity.h"
#include "Physics.h"
#include <map>

class EntityComponentSystem {
	public:
		void addEntity(Entity* e);
		void updatePhysics();
		void DrawEntities(float delta);
		void removeEntity(unsigned int ID);
		Entity* getEntity(unsigned int ID);
	private:
		std::map<int, Entity*> entds2;
};

#endif