#include "Entity.h"

Entity::Entity(Model* m, Shader* s, Shader* ws, Camera* c) {
	m_modeled = true;
	m_animated = false;
	m_dynamic = false;
	m_stenciled = false;
	m_surface = false;
	m_visible = true;
	mdl = m;
	skMdl = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = s;
	wireShader = ws;
	camera = c;
}

Entity::Entity(SkeletalModel* sm, Animator* m, Shader* s, Shader* ws, Camera* c) {
	m_modeled = false;
	m_animated = true;
	m_dynamic = false;
	m_stenciled = false;
	m_surface = false;
	m_visible = true;
	mdl = nullptr;
	skMdl = sm;
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
	m_stenciled = false;
	m_surface = false;
	m_visible = true;
	mdl = nullptr;
	skMdl = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = nullptr;
	wireShader = ws;
	camera = c;
}

// unexpected behavior if entity is deleted without using ECS::deleteEntity();
Entity::~Entity(){
	for (int i = 0; i < wires.size(); ++i) {
		delete wires[i];
	}
	wires.clear();
	if (body) {
		delete body->getMotionState();
		delete body;
	}
	if (m_animated) {
		delete skMdl;
		delete mator;
	}
	else if (m_modeled) {
		delete mdl;
	}
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

void Entity::addWireFrame(float halfWidth, float halfHeight, float halfLength) {
	wires.push_back(new Wire(glm::vec3(-halfWidth, halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, -halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, halfLength)));
	wires.push_back(new Wire(glm::vec3(-halfWidth, halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, -halfLength)));

	wires.push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, -halfHeight, -halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, -halfHeight, halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, -halfHeight, halfLength)));
	wires.push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, -halfHeight, -halfLength)));

	wires.push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, -halfLength), glm::vec3(-halfWidth, halfHeight, -halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, -halfLength)));
	wires.push_back(new Wire(glm::vec3(halfWidth, -halfHeight, halfLength), glm::vec3(halfWidth, halfHeight, halfLength)));
	wires.push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, halfLength)));
}

void Entity::DrawShadow(float delta) {
	if (!m_visible) return;
	if (m_animated) {
		mator->UpdateAnimation(delta);

		Globals::get().animShadowShader->Activate();
		auto transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			glUniformMatrix4fv(glGetUniformLocation(Globals::get().animShadowShader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		skMdl->Draw(*Globals::get().animShadowShader, *camera, translation, rotation, scale);
	}
	else if (m_modeled) {
		mdl->Draw(*Globals::get().shadowShader, *camera, translation, rotation, scale);
	}

}


void Entity::DrawStencil() {
	if (!m_visible) return;
	if (m_stenciled) {
		if (m_modeled) {
			glm::vec3 upScale = scale * 1.05f;
			mdl->Draw(*Globals::get().cellShader, *camera, translation, rotation, upScale);
		}
	}
}


void Entity::Draw() {
	if (!m_visible) return;
	if (m_animated) {
		shader->Activate();
		auto transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i)
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		skMdl->Draw(*shader, *camera, translation, rotation, scale);
	}
	else if (m_modeled) {
		if (m_surface) {
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glStencilMask(0xFF);
			mdl->Draw(*shader, *camera, translation, rotation, scale);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}
		else {
			mdl->Draw(*shader, *camera, translation, rotation, scale);
		}
	}

	if (Globals::get().drawWires) {
		for (int i = 0; i < wires.size(); i++) {
			wires[i]->Draw(*wireShader, *camera, translation, rotation, scale);
		}
	}

}

void Entity::updatePhysics() {
	if (m_dynamic) {
		btTransform trans;

		//if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);

		translation = glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		rotation = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
	}
}

void Entity::setTranslation(glm::vec3 translation) {
	if (!m_dynamic) Entity::translation = translation;
}

void Entity::setRotation(glm::quat rotation) {
	if (!m_dynamic) Entity::rotation = rotation;
}

void Entity::setScale(glm::vec3 scale) {
	if (!m_dynamic) Entity::scale = scale;
}
