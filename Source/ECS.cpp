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
	//clear component maps?
}

void ECS::registerComponents(Entity* e) {
	for (unsigned int i = 0; i < COMPONENT_BIT_COUNT; i++) {
		if (e->getBit(i)) {
			componentSets[i].linkEntity(e);
		}
	}
}

void ECS::registerComponent(Entity* e, unsigned int bit) {
	componentSets[bit].linkEntity(e);
}

void ECS::unregisterComponent(Entity* e, unsigned int bit) {
	componentSets[bit].unlinkEntity(e->getID());
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
	const auto& componentMap = componentSets[COMPONENT_BIT_DYNAMIC].getMap();

	for (auto it = componentMap.begin(); it != componentMap.end(); it++)
	{
		it->second->updatePhysicsState();
	}
}

void ECS::deleteEntity(unsigned int ID) {
	for (unsigned int i = 0; i < COMPONENT_BIT_COUNT; i++) {
		componentSets[i].unlinkEntity(ID);
	}

	auto iter = entMap.find(ID);
	if (iter != entMap.end())
	{
		Entity* ret = iter->second;
		if (ret) delete ret;
		entMap.erase(ID);
		availableIDs.push(ID);
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


void linkCameraUniforms(Shader& shader, Camera& camera) {
	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	camera.Matrix(shader, "camMatrix");
}

// WARNING: advances animations by delta!
void ECS::DrawEntities() {

	const auto& cam1Map = componentSets[COMPONENT_BIT_CAM1].getMap();
	const auto& cam2Map = componentSets[COMPONENT_BIT_CAM2].getMap();
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	linkCameraUniforms(*Globals::get().rigProgram, *Globals::get().camera);
	linkCameraUniforms(*Globals::get().lightProgram, *Globals::get().camera);
	linkCameraUniforms(*Globals::get().animProgram, *Globals::get().camera);
	linkCameraUniforms(*Globals::get().noTexAnimProgram, *Globals::get().camera);
	for (auto it = cam1Map.begin(); it != cam1Map.end(); it++)
	{
		it->second->Draw();
	}
	linkCameraUniforms(*Globals::get().rigProgram, *Globals::get().handCam);
	linkCameraUniforms(*Globals::get().lightProgram, *Globals::get().handCam);
	linkCameraUniforms(*Globals::get().animProgram, *Globals::get().handCam);
	linkCameraUniforms(*Globals::get().noTexAnimProgram, *Globals::get().handCam);
	for (auto it = cam2Map.begin(); it != cam2Map.end(); it++)
	{
		it->second->Draw();
	}
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}

void ECS::advanceEntityAnimations(float delta) {
	const auto& componentMap = componentSets[COMPONENT_BIT_ANIMATED].getMap();

	for (auto it = componentMap.begin(); it != componentMap.end(); it++)
	{
		it->second->advanceAnimation(delta);
	}
}

void ECS::DrawEntityShadows() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawShadow();
	}
}

void ECS::DrawEntityPointShadows() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawPointShadow();
	}
}


void ECS::DrawEntityStencils() {
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	const auto& componentMap = componentSets[COMPONENT_BIT_STENCIL].getMap();

	linkCameraUniforms(*Globals::get().cellShader, *Globals::get().camera);

	for (auto it = componentMap.begin(); it != componentMap.end(); it++)
	{
		it->second->DrawStencil();
	}
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}
