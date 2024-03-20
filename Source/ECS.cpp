#include "ECS.h"

ECS* ECS::instance = nullptr; // definition class variable

ECS::ECS() {
	for (unsigned int i = 1; i <= MAX_ENTITIES; i++) {
		availableIDs.push(i);
	}
}

ECS::~ECS() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		Entity* ret = it->second;
		if (ret) delete ret;
	}
	entMap.clear();
}

Entity* ECS::linkEntity(Entity* e) {
	if (availableIDs.empty()) return nullptr;
	unsigned int id = availableIDs.front();
	availableIDs.pop();
	e->setID(id);
	entMap[id] = e;
	return e;
}

void ECS::updatePhysics() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->updatePhysics();
	}
}

void ECS::deleteEntity(unsigned int ID) {
	auto iter = entMap.find(ID);
	if (iter != entMap.end())
	{
		Entity* ret = iter->second;
		if (ret) delete ret;
		entMap.erase(ID);
	} 
}

Entity* ECS::getEntity(unsigned int ID) {
	auto iter = entMap.find(ID);
	if (iter != entMap.end())
	{
		return iter->second;
	}
	return nullptr;
}


// WARNING: advances animations by delta!
void ECS::DrawEntities() {
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->Draw();
	}
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}

void ECS::DrawEntityShadows(float delta) {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawShadow(delta);
	}
}

void ECS::DrawEntityStencils() {
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawStencil();
	}
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}