#include "Entity.h"

Entity::Entity(Model* m, Shader* s, Camera* c) {
	setBit(COMPONENT_BIT_MODEL);
	m_surface = false;
	m_visible = true;
	mdl = m;
	skMdl = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = s;
	camera = c;
	transform = new Transform();
}

Entity::Entity(SkeletalModel* sm, Animator* m, Shader* s, Camera* c) {
	setBit(COMPONENT_BIT_ANIMATED);
	m_surface = false;
	m_visible = true;
	mdl = nullptr;
	skMdl = sm;
	mator = m;
	body = nullptr;
	shader = s;
	camera = c;
	transform = new Transform();
}

Entity::Entity(Camera* c) {
	m_surface = false;
	m_visible = true;
	mdl = nullptr;
	skMdl = nullptr;
	mator = nullptr;
	body = nullptr;
	shader = nullptr;
	camera = c;
	transform = new Transform();
}

// unexpected behavior if entity is deleted without using ECS::deleteEntity();
Entity::~Entity(){
	for (int i = 0; i < wires.size(); ++i) {
		delete wires[i];
	}
	wires.clear();
	if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
		Physics::get().getDynamicsWorld()->removeCollisionObject(body);
		delete body->getMotionState();
		delete body;
	}
	if (m_signature[COMPONENT_BIT_ANIMATED]) {
		delete skMdl;
		delete mator;
	}
	else if (m_signature[COMPONENT_BIT_MODEL]) {
		delete mdl;
	}
	delete transform;
	if (phystransform != nullptr) delete phystransform;
}

void Entity::addBody(btRigidBody* b) {
	if (body || !b) return;
	if (b->getMass() == 0.0f) {
		setBit(COMPONENT_BIT_STATIC);
		body = b;
	}
	else {
		setBit(COMPONENT_BIT_DYNAMIC);
		body = b;
		phystransform = new Transform();
	}
}

btRigidBody* Entity::getBody() {
	if (!m_signature[COMPONENT_BIT_DYNAMIC] && !m_signature[COMPONENT_BIT_STATIC]) return nullptr;
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

void Entity::setBit(std::size_t pos) {
	m_signature.set(pos);
}

void Entity::resetBit(std::size_t pos) {
	m_signature.reset(pos);
}

const bool& Entity::getBit(std::size_t pos) {
	return m_signature[pos];
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


void Entity::advanceAnimation(float delta) {
	if (m_signature[COMPONENT_BIT_ANIMATED]) {
		mator->UpdateAnimation(delta);
	}
}


void Entity::DrawShadow() {
	if (!m_visible) return;
	glm::mat4 finaltransform;
	if (m_signature[COMPONENT_BIT_DYNAMIC]) {
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

void Entity::DrawPointShadow() {
	if (!m_visible) return;
	if (m_type == "light") return;
	glm::mat4 finaltransform;
	if (m_signature[COMPONENT_BIT_DYNAMIC]) {
		finaltransform = phystransform->getMatrix() * transform->getMatrix();
	}
	else {
		finaltransform = transform->getMatrix();
	}
	if (m_signature[COMPONENT_BIT_ANIMATED]) {
		Globals::get().animPointShadowShader->Activate();
		const auto& transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			glUniformMatrix4fv(glGetUniformLocation(Globals::get().animPointShadowShader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		}
		skMdl->DrawShadow(*Globals::get().animPointShadowShader, finaltransform);
	}
	else if (m_signature[COMPONENT_BIT_MODEL]) {
		Globals::get().pointShadowShader->Activate();
		mdl->DrawShadow(*Globals::get().pointShadowShader, finaltransform);
	}
}


void Entity::DrawStencil() {
	if (!m_visible) return;
	if (m_signature[COMPONENT_BIT_STENCIL]) {
		if (m_signature[COMPONENT_BIT_MODEL]) {
			glm::vec3 oldScale = transform->getScale();
			glm::vec3 upScale = oldScale * 1.05f;
			transform->setScale(upScale);
			glm::mat4 finaltransform;
			glm::mat4 finalntransform;
			if (m_signature[COMPONENT_BIT_DYNAMIC]) {
				finaltransform = phystransform->getMatrix() * transform->getMatrix();
				finalntransform = phystransform->getNormalMatrix() * transform->getNormalMatrix();
			}
			else {
				finaltransform = transform->getMatrix();
				finalntransform = transform->getNormalMatrix();
			}
			mdl->Draw(*Globals::get().cellShader, *camera, finaltransform, finalntransform);
			transform->setScale(oldScale);
		}
	}
}

// update finaltransform?
void Entity::Draw() {
	if (!m_visible) return;
	glm::mat4 finaltransform;
	glm::mat4 finalntransform;
	if (m_signature[COMPONENT_BIT_DYNAMIC]) {
		finaltransform = phystransform->getMatrix() * transform->getMatrix();
		finalntransform = phystransform->getNormalMatrix() * transform->getNormalMatrix();
	}
	else {
		finaltransform = transform->getMatrix();
		finalntransform = transform->getNormalMatrix();
	}
	if (m_signature[COMPONENT_BIT_ANIMATED]) {
		shader->Activate();
		const auto& transforms = mator->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			glUniformMatrix4fv(glGetUniformLocation(shader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		}
		skMdl->Draw(*shader, *camera, finaltransform, finalntransform);
	}
	else if (m_signature[COMPONENT_BIT_MODEL]) {
		if (m_surface) {
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glStencilMask(0xFF);
			mdl->Draw(*shader, *camera, finaltransform, finalntransform);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}
		else {
			mdl->Draw(*shader, *camera, finaltransform, finalntransform);
		}
	}
	
	if (Globals::get().drawWires) {
		for (int i = 0; i < wires.size(); i++) {
			wires[i]->Draw(*Globals::get().wireShader, *camera, finaltransform);
		}
	}


}

void Entity::updatePhysicsState() {
	if (m_signature[COMPONENT_BIT_DYNAMIC]) {
		btTransform trans;

		//if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);

		phystransform->setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
		phystransform->setRotation(glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()));
	}
}

