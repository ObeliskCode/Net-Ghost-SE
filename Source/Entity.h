#ifndef ENTITY_H
#define ENTITY_H

#include "Model.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Skeleton.h"
#include "Animator.h"
#include "Globals.h"

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
		void addWireFrame(float halfWidth, float halfHeight, float halfLength);
		void updatePhysics();
		void DrawShadow(float delta);
		void DrawStencil();
		void Draw();
		void addBody(btRigidBody* b);
		btRigidBody* getBody();

		// do not use these if entity is dynamic!!
		void setTranslation(glm::vec3 translation);
		void setRotation(glm::quat rotation);
		void setScale(glm::vec3 scale);

		bool m_visible;
		bool m_stenciled;
		bool m_surface;

	private:
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

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