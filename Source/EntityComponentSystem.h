#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include "Entity.h"
#include "Physics.h"

class EntityComponentSystem {
	public:
		void addEntity(Entity e);
		void updatePhysics();
		void DrawEntities(float delta);
		//remEnt? 
	private:
		std::vector<Entity> entities;
};

#endif