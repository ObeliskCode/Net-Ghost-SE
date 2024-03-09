#ifndef ENTITY_H
#define ENTITY_H

#include "Model.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Animation.h"
#include "Animator.h"

#include "btBulletDynamicsCommon.h"

class Entity {
	public:
		Entity(Shader* ws, Camera* c);
		Entity(Model* m, Shader* s, Shader* ws, Camera* c);
		Entity(SkeletalModel* sm, Animation* a, Animator* m, Shader* s, Shader* ws, Camera* c);

		void addWire(Wire* w);
		void updatePhysics();
		void Draw(float delta);
		void addBody(btRigidBody* b);
		btRigidBody* getBody();
	private:
		// ID? name?
		bool m_animated;
		bool m_modeled;
		bool m_dynamic;
		Model* mdl;
		SkeletalModel* skMdl;
		Animation* anim;
		Animator* mator;
		std::vector<Wire*> wires;
		btRigidBody* body;
		Shader* shader;
		Shader* wireShader;
		Camera* camera;
};


#endif