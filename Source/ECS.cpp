#include "ECS.h"

#define NO_WIRES

#ifdef NO_WIRES
#define WIRES_DISABLED true
#else
#define WIRES_DISABLED false
#endif


ECS* ECS::instance = nullptr; // definition class variable

ECS::ECS() {
	for (unsigned int i = 1; i <= MAX_ENTITIES; i++) {
		availableIDs.push(i);
	}
}

ECS::~ECS() {
	//TODO
}

// TODO: add safety checks to ECS!

unsigned int ECS::createEntity() {
	if (availableIDs.empty()) return NULL;
	unsigned int id = availableIDs.front();
	availableIDs.pop();
	entMap[id] = Entity(id);
	return id;
}
void ECS::addShader(unsigned int ID, Shader* sh) {
	cset_shader.linkEntity(ID, sh);
	Entity e = entMap[ID];
	e.shader_flag = 1;
	entMap[ID] = e;
}
void ECS::addCamera(unsigned int ID, Camera* cam) {
	cset_camera.linkEntity(ID, cam);
	Entity e = entMap[ID];
	e.camera_flag = 1;
	entMap[ID] = e;
}
void ECS::addTransform(unsigned int ID, Transform* transf) {
	cset_transform.linkEntity(ID, transf);
	Entity e = entMap[ID];
	e.transform_flag = 1;
	entMap[ID] = e;
}
void ECS::addModel(unsigned int ID, Model* mdl) {
	cset_model.linkEntity(ID, mdl);
	Entity e = entMap[ID];
	e.model_flag = 1;
	entMap[ID] = e;
}
void ECS::addSkModel(unsigned int ID, SkeletalModel* skmdl) {
	cset_skmodel.linkEntity(ID, skmdl);
	Entity e = entMap[ID];
	e.skmodel_flag = 1;
	entMap[ID] = e;
}
void ECS::addAnimator(unsigned int ID, Animator* mator) {
	cset_animator.linkEntity(ID, mator);
	Entity e = entMap[ID];
	e.animator_flag = 1;
	entMap[ID] = e;
}
void ECS::addPhysBody(unsigned int ID, btRigidBody* b) {
	cset_body.linkEntity(ID, b);
	Entity e = entMap[ID];
	e.phystransform_flag = 1;
    if (b->getMass() == 0.0f) {
		e.dynamic_flag = 0;
	}
	else {
        e.dynamic_flag = 1;
    }
	entMap[ID] = e;
	//updatePhysicsState(); // is this necessary?
}
void ECS::addPhysTransform(unsigned int ID, Transform* phystransf) {
	cset_phystransform.linkEntity(ID, phystransf);
	Entity e = entMap[ID];
	e.phystransform_flag = 1;
	entMap[ID] = e;
}
void ECS::addWire(unsigned int ID, Wire* w) {
    //TODO
}
void ECS::addWireFrame(unsigned int ID, float halfWidth, float halfHeight, float halfLength) {
    //TODO
}

Entity ECS::getEntity(unsigned int ID) {
	auto iter = entMap.find(ID);
	if (iter != entMap.end())
	{
		return iter->second;
	}
	return Entity(NULL);
}

void ECS::updateEntity(Entity e){
    entMap[e.m_id] = e;
}

void ECS::deleteEntity(unsigned int ID) {

	auto iter = entMap.find(ID);
	if (iter != entMap.end())
	{
		Entity ret = iter->second;
		if (ret.camera_flag) cset_camera.unlinkEntity(ID);
		if (ret.shader_flag) cset_shader.unlinkEntity(ID);
		if (ret.transform_flag) {
            Transform* transPtr = cset_transform.getMem();
            delete transPtr;
            cset_transform.unlinkEntity(ID);
		}
		if (ret.model_flag) {
            Model* mdlPtr = cset_model.getMem();
            delete mdlPtr;
            cset_model.unlinkEntity(ID);
		}
		if (ret.skmodel_flag) {
            SkeletalModel* skmdlPtr = cset_skmodel.getMem();
            delete skmdlPtr;
            cset_skmodel.unlinkEntity(ID);
		}
		if (ret.animator_flag) {
            Animator* animPtr = cset_animator.getMem();
            delete animPtr;
            cset_animator.unlinkEntity(ID);
		}
		if (ret.physbody_flag) {
            btRigidBody* bodyPtr = cset_body.getMem();
            Physics::get().getDynamicsWorld()->removeCollisionObject(bodyPtr);
            delete bodyPtr->getMotionState();
            delete bodyPtr;
            cset_body.unlinkEntity(ID);
		}
		if (ret.phystransform_flag) {
            Transform* phystransPtr = cset_phystransform.getMem();
            delete phystransPtr;
            cset_phystransform.unlinkEntity(ID);
		}

		entMap.erase(ID);
		availableIDs.push(ID);
	}
}

void ECS::syncPhysics() {
	for (auto it = cset_body.entSet.begin(); it != cset_body.entSet.end(); it++)
	{
        unsigned int ID = *it;
        btRigidBody* body = cset_body.getMem(ID);
        Entity e = getEntity(ID);

		btTransform trans;
		//if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);

		Transform* pt = cset_phystransform.getMem(ID);
		pt->setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
		pt->setRotation(glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()));

	}
}

void linkCameraUniforms(Shader& shader, Camera& camera) {
	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	camera.Matrix(shader, "camMatrix");
}

// TODO
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
	for (auto it = cset_animator.entSet.begin(); it != cset_animator.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Animator* animPtr = cset_animator.getMem(ID);
        animPtr->UpdateAnimation(delta);
	}
}

/*

void Entity::DrawShadow() {
	if (!m_visible) return;
	glm::mat4 finaltransform;
	if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
		finaltransform = phystransform->getMatrix() * transform->getMatrix();
	}
	else {
		finaltransform = transform->getMatrix();
	}
	if (m_signature[COMPONENT_BIT_ANIMATED]) {
		Globals::get().animShadowShader->Activate();
		const auto& transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			glUniformMatrix4fv(glGetUniformLocation(Globals::get().animShadowShader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		}
		skMdl->DrawShadow(*Globals::get().animShadowShader, finaltransform);
	}
	else if (m_signature[COMPONENT_BIT_MODEL]) {
		Globals::get().shadowShader->Activate();
		mdl->DrawShadow(*Globals::get().shadowShader, finaltransform);
	}

}
*/
void ECS::DrawEntityShadows() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawShadow();
	}
}

// TODO
void ECS::DrawEntityPointShadows() {
	for (auto it = entMap.begin(); it != entMap.end(); it++)
	{
		it->second->DrawPointShadow();
	}
}

// TODO
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
