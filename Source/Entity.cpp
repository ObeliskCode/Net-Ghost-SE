#include "Entity.h"

// code to be refactored

/*
void Entity::addWire(Wire* w) {
	if (WIRES_DISABLED) {
		delete w;
	}
	else {
		wires.push_back(w);
	}
}

void Entity::addWireFrame(float halfWidth, float halfHeight, float halfLength) {
	if (!WIRES_DISABLED) {
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
}



void Entity::DrawPointShadow() {
	if (!m_visible) return;
	if (m_type == "light") return;
	glm::mat4 finaltransform;
	if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
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
			if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
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
	if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
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

	if (m_signature[COMPONENT_BIT_DYNAMIC] || m_signature[COMPONENT_BIT_STATIC]) {
		if (Globals::get().drawWires) {
			for (int i = 0; i < wires.size(); i++) {
				glm::mat4 tran = phystransform->getMatrix();
				wires[i]->Draw(*Globals::get().wireShader, *camera, tran);
			}
		}
	}
	else {
		if (Globals::get().drawWires) {
			for (int i = 0; i < wires.size(); i++) {
				wires[i]->Draw(*Globals::get().wireShader, *camera, finaltransform);
			}
		}
	}


}

*/
