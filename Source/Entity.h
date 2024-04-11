#ifndef ENTITY_H
#define ENTITY_H

#include "Model.h"
#include "Physics.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Skeleton.h"
#include "Animator.h"
#include "Globals.h"

#include "bullet/btBulletDynamicsCommon.h"

#include <bitset>

// FIX so entity has ID at creation!
class Entity {
	public:
		Entity(Camera* c);
		Entity(Model* m, Shader* s, Camera* c);
		Entity(SkeletalModel* sm, Animator* m, Shader* s, Camera* c);
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

		void setBit(std::size_t pos);
		void resetBit(std::size_t pos);
		const bool& getBit(std::size_t pos); // should this be const reference lol

		// do not use these if entity is dynamic!!
		void setTranslation(glm::vec3 translation);
		void setRotation(glm::quat rotation);
		void setScale(glm::vec3 scale);

		bool m_visible;
		bool m_surface;

	private:
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		#define COMPONENT_BIT_MODEL 0
		#define COMPONENT_BIT_ANIMATED 1
		#define COMPONENT_BIT_DYNAMIC 2
		#define COMPONENT_BIT_STATIC 3
		#define COMPONENT_BIT_STENCIL 4
		#define COMPONENT_BIT_COUNT 5
		std::bitset<COMPONENT_BIT_COUNT> m_signature;

		unsigned int m_id;
		std::string m_type;

		//std::unordered_map<unsigned int, void*> pointerBuffer; // make entity class into typless pointer container? <pointer_type_enum, void_casted_pointer>
		
		Model* mdl;
		SkeletalModel* skMdl;
		Animator* mator;
		std::vector<Wire*> wires;
		btRigidBody* body;
		Shader* shader;
		Camera* camera;
};

#endif