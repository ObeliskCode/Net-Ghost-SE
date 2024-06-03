#include "ECS.h"

//#define NO_WIRES

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
	for (auto it = entMap.begin(); it != entMap.end(); it++) {
        deleteEntity(it->first);
	}
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
// warning: code may be broken, also may soon be removed
void ECS::addCamera(unsigned int ID, Camera* cam) {
	cset_camera.linkEntity(ID, cam);
	Entity e = entMap[ID];
	e.camera_flag = 1;
	if (cam == globals.camera){
        e.m_camera = 1;
        cam1Set.insert(ID);
	} else if (cam == globals.handCam){
        e.m_camera = 2;
        cam2Set.insert(ID);
	}
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
	e.physbody_flag = 1;
    if (b->getMass() == 0.0f) {
		e.dynamic_flag = 0;
	}
	else {
        e.dynamic_flag = 1;
    }
	entMap[ID] = e;
}
void ECS::addPhysTransform(unsigned int ID, Transform* phystransf) {
	cset_phystransform.linkEntity(ID, phystransf);
	Entity e = entMap[ID];
	e.phystransform_flag = 1;
	entMap[ID] = e;
}
void ECS::addWire(unsigned int ID, Wire* w) {
    if (WIRES_DISABLED) {
		delete w;
	}
	else {
		cset_wire.getMem(ID).push_back(w);
	}
}
void ECS::addWireFrame(unsigned int ID, float halfWidth, float halfHeight, float halfLength) {
    if (!WIRES_DISABLED) {
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, -halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, -halfLength)));

		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, -halfHeight, -halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, -halfHeight, halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, -halfHeight, halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, -halfHeight, -halfLength)));

		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, -halfLength), glm::vec3(-halfWidth, halfHeight, -halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, -halfHeight, -halfLength), glm::vec3(halfWidth, halfHeight, -halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(halfWidth, -halfHeight, halfLength), glm::vec3(halfWidth, halfHeight, halfLength)));
		cset_wire.getMem(ID).push_back(new Wire(glm::vec3(-halfWidth, -halfHeight, halfLength), glm::vec3(-halfWidth, halfHeight, halfLength)));
	}
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
		if (ret.shader_flag) cset_shader.unlinkEntity(ID);
		if (ret.camera_flag) {
            cset_camera.unlinkEntity(ID);
            if (ret.m_camera == 1){
                cam1Set.erase(ID);
            } else if (ret.m_camera == 2){
                cam2Set.erase(ID);
            }
        }
		if (ret.transform_flag) {
            Transform* transPtr = cset_transform.getMem(ID);
            delete transPtr;
            cset_transform.unlinkEntity(ID);
		}
		if (ret.model_flag) {
            Model* mdlPtr = cset_model.getMem(ID);
            delete mdlPtr;
            cset_model.unlinkEntity(ID);
		}
		if (ret.skmodel_flag) {
            SkeletalModel* skmdlPtr = cset_skmodel.getMem(ID);
            delete skmdlPtr;
            cset_skmodel.unlinkEntity(ID);
		}
		if (ret.animator_flag) {
            Animator* animPtr = cset_animator.getMem(ID);
            delete animPtr;
            cset_animator.unlinkEntity(ID);
		}
		if (ret.physbody_flag) {
            btRigidBody* bodyPtr = cset_body.getMem(ID);
            Physics::get().getDynamicsWorld()->removeCollisionObject(bodyPtr);
            Physics::get().m_EntityMap.erase(bodyPtr); // new addition?
            delete bodyPtr->getMotionState();
            delete bodyPtr;
            cset_body.unlinkEntity(ID);
		}
		if (ret.phystransform_flag) {
            Transform* phystransPtr = cset_phystransform.getMem(ID);
            delete phystransPtr;
            cset_phystransform.unlinkEntity(ID);
		}
		for (auto it = cset_wire.getMem(ID).begin(); it != cset_wire.getMem(ID).end(); it++ ) {
            delete *it;
		}
		cset_wire.getMem(ID).clear();
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
        if (body && body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);

            Transform* pt = cset_phystransform.getMem(ID);
            pt->setTranslation(glm::vec3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
            pt->setRotation(glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ()));
        }
	}
}

void linkCameraUniforms(Shader& shader, Camera& camera) {
	shader.Activate();
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	camera.Matrix(shader, "camMatrix");
}

void ECS::DrawScreenEntity(unsigned int ID) {

	linkCameraUniforms(*globals.rigProgram, *globals.handCam);
	linkCameraUniforms(*globals.lightProgram, *globals.handCam);
	linkCameraUniforms(*globals.animProgram, *globals.handCam);
	linkCameraUniforms(*globals.noTexAnimProgram, *globals.handCam);
    Entity e = getEntity(ID);
    if (!e.visible_flag) {
       return;
    }
    if (e.stencil_flag) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }

    Transform* phystrf = cset_phystransform.getMem(ID);
    Transform* trf = cset_transform.getMem(ID);
    
    glm::mat4 finaltransform;
    glm::mat4 finalntransform;
    if (e.phystransform_flag) {
        finaltransform = phystrf->getMatrix() * trf->getMatrix();
        finalntransform = phystrf->getNormalMatrix() * trf->getNormalMatrix();
    }
    else {
        finaltransform = trf->getMatrix();
        finalntransform = trf->getNormalMatrix();
    }

    if (e.animator_flag) {
        SkeletalModel* skmdl = cset_skmodel.getMem(ID);
        Shader* sh = cset_shader.getMem(ID);
        sh->Activate();
        const auto& transforms = cset_animator.getMem(ID)->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            glUniformMatrix4fv(glGetUniformLocation(sh->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
        }
        skmdl->Draw(*sh, *globals.handCam, finaltransform, finalntransform);
    } else if (e.model_flag) {
        Model* mdl = cset_model.getMem(ID);
        if (e.surface_flag) {
            //glStencilFunc(GL_ALWAYS, 0, 0xFF);
            //glStencilMask(0xFF);
            mdl->Draw(*cset_shader.getMem(ID), *globals.handCam, finaltransform, finalntransform);
            //glStencilFunc(GL_ALWAYS, 1, 0xFF);
            //glStencilMask(0xFF);
        }
        else {
            mdl->Draw(*cset_shader.getMem(ID), *globals.handCam, finaltransform, finalntransform);
        }
    }

    if (e.stencil_flag) {
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
    }
}

void ECS::DrawEntity(unsigned int ID) {
	linkCameraUniforms(*globals.rigProgram, *globals.camera);
	linkCameraUniforms(*globals.lightProgram, *globals.camera);
	linkCameraUniforms(*globals.animProgram, *globals.camera);
	linkCameraUniforms(*globals.noTexAnimProgram, *globals.camera);
    Entity e = getEntity(ID);
    if (!e.visible_flag) {
        return;
    }
    if (e.stencil_flag) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }

    Transform* phystrf = cset_phystransform.getMem(ID);
    Transform* trf = cset_transform.getMem(ID);

    glm::mat4 finaltransform;
    glm::mat4 finalntransform;
    if (e.phystransform_flag) {
        finaltransform = phystrf->getMatrix() * trf->getMatrix();
        finalntransform = phystrf->getNormalMatrix() * trf->getNormalMatrix();
    }
    else {
        finaltransform = trf->getMatrix();
        finalntransform = trf->getNormalMatrix();
    }

    if (e.animator_flag) {
        SkeletalModel* skmdl = cset_skmodel.getMem(ID);
        Shader* sh = cset_shader.getMem(ID);
        sh->Activate();
        const auto& transforms = cset_animator.getMem(ID)->GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i) {
            glUniformMatrix4fv(glGetUniformLocation(sh->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
        }
        skmdl->Draw(*sh, *globals.camera, finaltransform, finalntransform);
    } else if (e.model_flag) {
        Model* mdl = cset_model.getMem(ID);
        if (e.surface_flag) {
            //glStencilFunc(GL_ALWAYS, 0, 0xFF);
            //glStencilMask(0xFF);
            mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
            //glStencilFunc(GL_ALWAYS, 1, 0xFF);
            //glStencilMask(0xFF);
        }
        else {
            mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
        }
    }

    if (e.stencil_flag) {
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
    }

	const auto& wires = cset_wire.getMem(ID);
	if (e.phystransform_flag) {
        glm::mat4 tran = phystrf->getMatrix();
        for (int i = 0; i < wires.size(); i++) {
            wires[i]->Draw(*globals.wireShader, *cset_camera.getMem(ID), tran);
        }
	}
	else {
        for (int i = 0; i < wires.size(); i++) {
            wires[i]->Draw(*globals.wireShader, *cset_camera.getMem(ID), finaltransform);
        }
	}
}

// TODO: this function needs to be updated to support multiple cameras!
void ECS::DrawEntities() {
	linkCameraUniforms(*globals.rigProgram, *globals.camera);
	linkCameraUniforms(*globals.lightProgram, *globals.camera);
	linkCameraUniforms(*globals.animProgram, *globals.camera);
	linkCameraUniforms(*globals.noTexAnimProgram, *globals.camera);

	for (auto it = entMap.begin(); it != entMap.end(); it++){
        unsigned int ID = it->first;
        Entity e = it->second;

        if (!e.visible_flag) continue;

        if (e.stencil_flag) {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
        }

        Transform* phystrf = cset_phystransform.getMem(ID);
        Transform* trf = cset_transform.getMem(ID);

        glm::mat4 finaltransform;
        glm::mat4 finalntransform;
        if (e.phystransform_flag) {
            finaltransform = phystrf->getMatrix() * trf->getMatrix();
            finalntransform = phystrf->getNormalMatrix() * trf->getNormalMatrix();
        }
        else {
            finaltransform = trf->getMatrix();
            finalntransform = trf->getNormalMatrix();
        }

        if (e.animator_flag) {
            SkeletalModel* skmdl = cset_skmodel.getMem(ID);
            Shader* sh = cset_shader.getMem(ID);
            sh->Activate();
            const auto& transforms = cset_animator.getMem(ID)->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                glUniformMatrix4fv(glGetUniformLocation(sh->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
            }
            skmdl->Draw(*sh, *globals.camera, finaltransform, finalntransform);
        } else

        if (e.model_flag) {
            Model* mdl = cset_model.getMem(ID);
            if (!e.shader_flag) std::cerr << "no shader?" << std::endl;
            if (e.surface_flag) {
                //glStencilFunc(GL_ALWAYS, 0, 0xFF);
                //glStencilMask(0xFF);
                mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
                //glStencilFunc(GL_ALWAYS, 1, 0xFF);
                //glStencilMask(0xFF);
            }
            else {
                mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
            }
        }

        if (e.stencil_flag) {
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilMask(0xFF);
        }

        if (globals.drawWires){
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            const auto& wires = cset_wire.getMem(ID);
            if (e.phystransform_flag) {
                glm::mat4 tran = phystrf->getMatrix();
                for (int i = 0; i < wires.size(); i++) {
                    wires[i]->Draw(*globals.wireShader, *cset_camera.getMem(ID), tran);
                }
            }
            else {
                for (int i = 0; i < wires.size(); i++) {
                    wires[i]->Draw(*globals.wireShader, *cset_camera.getMem(ID), finaltransform);
                }
            }
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilMask(0xFF);
        }
	}
}

/*
// TODO : finish wire code
void ECS::DrawEntities() {
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	linkCameraUniforms(*globals.rigProgram, *globals.camera);
	linkCameraUniforms(*globals.lightProgram, *globals.camera);
	linkCameraUniforms(*globals.animProgram, *globals.camera);
	linkCameraUniforms(*globals.noTexAnimProgram, *globals.camera);
	for (auto it = cam1Set.begin(); it != cam1Set.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        glm::mat4 finaltransform;
        glm::mat4 finalntransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
            finalntransform = cset_phystransform.getMem(ID)->getNormalMatrix() * cset_transform.getMem(ID)->getNormalMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
            finalntransform = cset_transform.getMem(ID)->getNormalMatrix();
        }

        if (e.animator_flag) {
            SkeletalModel* skmdl = cset_skmodel.getMem(ID);
            cset_shader.getMem(ID)->Activate();
            const auto& transforms = cset_animator.getMem(ID)->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                glUniformMatrix4fv(glGetUniformLocation(cset_shader.getMem(ID)->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
            }
            skmdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
        } else

        if (e.model_flag) {
            Model* mdl = cset_model.getMem(ID);
            if (!e.shader_flag) std::cerr << "no shader?" << std::endl;
            if (e.surface_flag) {
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
                glStencilMask(0xFF);
                mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
            }
            else {
                mdl->Draw(*cset_shader.getMem(ID), *globals.camera, finaltransform, finalntransform);
            }
        }

	}
	std::cerr << "hello" << std::endl;
	linkCameraUniforms(*globals.rigProgram, *globals.handCam);
	linkCameraUniforms(*globals.lightProgram, *globals.handCam);
	linkCameraUniforms(*globals.animProgram, *globals.handCam);
	linkCameraUniforms(*globals.noTexAnimProgram, *globals.handCam);
	for (auto it = cam2Set.begin(); it != cam2Set.end(); it++)
	{
		unsigned int ID = *it;
		Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        glm::mat4 finaltransform;
        glm::mat4 finalntransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
            finalntransform = cset_phystransform.getMem(ID)->getNormalMatrix() * cset_transform.getMem(ID)->getNormalMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
            finalntransform = cset_transform.getMem(ID)->getNormalMatrix();
        }

        if (e.animator_flag) {
            SkeletalModel* skmdl = cset_skmodel.getMem(ID);
            cset_shader.getMem(ID)->Activate();
            const auto& transforms = cset_animator.getMem(ID)->GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                glUniformMatrix4fv(glGetUniformLocation(cset_shader.getMem(ID)->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
            }
            skmdl->Draw(*cset_shader.getMem(ID), *globals.handCam, finaltransform, finalntransform);
        } else if (e.model_flag) {
            Model* mdl = cset_model.getMem(ID);
            if (e.surface_flag) {
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
                glStencilMask(0xFF);
                mdl->Draw(*cset_shader.getMem(ID), *globals.handCam, finaltransform, finalntransform);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
            }
            else {
                mdl->Draw(*cset_shader.getMem(ID), *globals.handCam, finaltransform, finalntransform);
            }
        }
	}
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}
*/

void ECS::advanceEntityAnimations(float delta) {
	for (auto it = cset_animator.entSet.begin(); it != cset_animator.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Animator* animPtr = cset_animator.getMem(ID);
        animPtr->UpdateAnimation(delta);
	}
}

void ECS::DrawEntityShadows() {
	for (auto it = cset_skmodel.entSet.begin(); it != cset_skmodel.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        glm::mat4 finaltransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
        }
        SkeletalModel* skmdl = cset_skmodel.getMem(ID);
        Animator* anim =  cset_animator.getMem(ID);
        globals.animShadowShader->Activate();
		const auto& transforms = anim->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			glUniformMatrix4fv(glGetUniformLocation(globals.animShadowShader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		}
		skmdl->DrawShadow(*globals.animShadowShader, finaltransform);
	}
	for (auto it = cset_model.entSet.begin(); it != cset_model.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        glm::mat4 finaltransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
        }
        Model* mdl = cset_model.getMem(ID);
		globals.shadowShader->Activate();
		mdl->DrawShadow(*globals.shadowShader, finaltransform);
	}
}

void ECS::DrawEntityPointShadows() {
    for (auto it = cset_skmodel.entSet.begin(); it != cset_skmodel.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        if (e.light_flag) continue;
        glm::mat4 finaltransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
        }
        SkeletalModel* skmdl = cset_skmodel.getMem(ID);
        Animator* anim =  cset_animator.getMem(ID);
        globals.animPointShadowShader->Activate();
		const auto& transforms = anim->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			glUniformMatrix4fv(glGetUniformLocation(globals.animPointShadowShader->ID, ("finalBonesMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, &transforms[i][0][0]);
		}
		skmdl->DrawShadow(*globals.animPointShadowShader, finaltransform);
	}
	for (auto it = cset_model.entSet.begin(); it != cset_model.entSet.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        if (e.light_flag) continue;
        glm::mat4 finaltransform;
        if (e.phystransform_flag) {
            finaltransform = cset_phystransform.getMem(ID)->getMatrix() * cset_transform.getMem(ID)->getMatrix();
        }
        else {
            finaltransform = cset_transform.getMem(ID)->getMatrix();
        }
        Model* mdl = cset_model.getMem(ID);
		globals.pointShadowShader->Activate();
		mdl->DrawShadow(*globals.pointShadowShader, finaltransform);
	}
}

void ECS::DrawEntityStencils() {
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	linkCameraUniforms(*globals.cellShader, *globals.camera);

	for (auto it = stencilSet.begin(); it != stencilSet.end(); it++)
	{
        unsigned int ID = *it;
        Entity e = getEntity(ID);
        if (!e.visible_flag) continue;
        if (!e.model_flag) continue;

        Model* mdl = cset_model.getMem(ID);

        Transform* trf = cset_transform.getMem(ID);

        glm::vec3 oldScale = cset_transform.getMem(ID)->getScale();
        glm::vec3 upScale = oldScale * 1.05f;
        trf->setScale(upScale);

        Transform* phystrf = cset_phystransform.getMem(ID);
        
        glm::mat4 finaltransform;
        glm::mat4 finalntransform;
        if (e.phystransform_flag) {
            finaltransform = phystrf->getMatrix() * trf->getMatrix();
            finalntransform = phystrf->getNormalMatrix() * trf->getNormalMatrix();
        }
        else {
            finaltransform = trf->getMatrix();
            finalntransform = trf->getNormalMatrix();
        }

        mdl->Draw(*globals.cellShader, *globals.camera, finaltransform, finalntransform);
        trf->setScale(oldScale);

	}
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
}
