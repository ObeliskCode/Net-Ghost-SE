#ifndef ENTITY_H
#define ENTITY_H

#include "Model.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Skeleton.h"
#include "Animator.h"

#include "btBulletDynamicsCommon.h"

// FIX so entity has ID at creation!
class Entity {
	public:
		Entity(Shader* ws, Camera* c);
		Entity(Model* m, Shader* s, Shader* ws, Camera* c);
		Entity(SkeletalModel* sm, Animator* m, Shader* s, Shader* ws, Camera* c);
		~Entity();

		void setType(std::string type);
		std::string getType();
		void setID(unsigned int ID);
		unsigned int getID();
		void addWire(Wire* w);
		void updatePhysics();
		void Draw(float delta);
		void addBody(btRigidBody* b);
		btRigidBody* getBody();

		bool m_visible;

	private:
		unsigned int m_id;
		std::string m_type;
		bool m_animated;
		bool m_modeled;
		bool m_dynamic;
		Model* mdl;
		SkeletalModel* skMdl;
		Animator* mator;
		std::vector<Wire*> wires;
		btRigidBody* body;
		Shader* shader;
		Shader* wireShader;
		Camera* camera;
};

#endif