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
	auto iter = entds2.find(ID);
	if (iter != entds2.end())
	{
		return iter->second;
	}
	return nullptr;
}


// WARNING: advances animations by delta!
void ECS::DrawEntities() {
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->Draw();
	}
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}

void ECS::DrawEntityShadows(float delta) {
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->DrawShadow(delta);
	}
}

void ECS::DrawEntityStencils() {
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	for (auto it = entds2.begin(); it != entds2.end(); it++)
	{
		it->second->DrawStencil();
	}
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}