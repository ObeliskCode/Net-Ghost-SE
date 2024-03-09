#include "Entity.h"

Entity::Entity(Model* m, Shader* s, Shader* ws, Camera* c) {
	m_modeled = true;
	m_animated = false;
	m_dynamic = false;
	mdl = m;
	skMdl = nullptr;
	anim = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = s;
	wireShader = ws;
	camera = c;
}

Entity::Entity(SkeletalModel* sm, Animation* a, Animator* m, Shader* s, Shader* ws, Camera* c) {
	m_modeled = false;
	m_animated = true;
	m_dynamic = false;
	mdl = nullptr;
	skMdl = sm;
	anim = a;
	mator = m;
	body = nullptr;
	shader = s;
	wireShader = ws;
	camera = c;
}

Entity::Entity(Shader* ws, Camera* c) {
	m_modeled = false;
	m_animated = false;
	m_dynamic = false;
	mdl = nullptr;
	skMdl = nullptr;
	anim = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = nullptr;
	wireShader = ws;
	camera = c;
}

Entity::~Entity(){
	for (int i = 0; i < wires.size(); ++i) {
		delete wires[i];
	}
	wires.clear();
	// what should be deleted here!?
}

void Entity::addBody(btRigidBody* b) {
	m_dynamic = true;
	body = b;
}

btRigidBody* Entity::getBody() {
	if (!m_dynamic) return nullptr;
	return body;
}

void Entity::setType(std::string type) {
	m_type = type;
}

std::string Entity::getType() {
	return m_type;
}

void Entity::setID(unsigned int ID) {
	m_id = ID;
}

unsigned int Entity::getID() {
	return m_id;
}

void Entity::addWire(Wire* w) {
	wires.push_back(w);
}

void Entity::Draw(float delta) {
	if (m_animated) {
		shader->Activate();
		mator->UpdateAnimation(delta);

		auto transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);

		skMdl->Draw(*shader, *camera);
	}
	else if (m_modeled) {
		mdl->Draw(*shader, *camera);
	}

	for (int i = 0; i < wires.size(); i++) {
		wires[i]->Draw(*wireShader, *camera);
	}

}

void Entity::updatePhysics() {
	if (m_dynamic) {
		btTransform trans;

		//if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);

		glm::vec3 pos = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		glm::quat rot = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());

		for (int i = 0; i < wires.size(); ++i) {
			wires[i]->setTranslation(pos);
			wires[i]->setRotation(rot);
		}

		if (m_animated) {
			skMdl->setTranslation(pos);
			skMdl->setRotation(rot);
		}
		else if (m_modeled) {
			mdl->setTranslation(pos);
			mdl->setRotation(rot);
		}
	}
}