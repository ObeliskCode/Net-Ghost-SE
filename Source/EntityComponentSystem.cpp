#include "EntityComponentSystem.h"

void EntityComponentSystem::addEntity(Entity e) {
	entities.push_back(e);
}

void EntityComponentSystem::updatePhysics() {
	for (int i = 0; i < entities.size(); i++) {
		entities[i].updatePhysics();
	}

}

// WARNING: advances animations by delta!
void EntityComponentSystem::DrawEntities(float delta) {
	for (int i = 0; i < entities.size(); i++) {
		entities[i].Draw(delta);
	}
}