#ifndef ENTITY_H
#define ENTITY_H

#include "Model.h"
#include "Physics.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Skeleton.h"
#include "Animator.h"
#include "Globals.h"
#include "Transform.h"

#include "btBulletDynamicsCommon.h"

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

		void setBit(std::size_t pos);
		void resetBit(std::size_t pos);
		bool getBit(std::size_t pos); // should this be const reference lol // NO!!!

		void addWire(Wire* w);
		void addWireFrame(float halfWidth, float halfHeight, float halfLength);
		void updatePhysicsState();
		void advanceAnimation(float delta);
		void DrawPointShadow();
		void DrawShadow();
		void DrawStencil();
		void Draw();
		void addBody(btRigidBody* b);
		btRigidBody* getBody();

		Transform* transform;
		Shader* shader;
		Camera* camera;

		bool m_visible;
		bool m_surface;

	private:


		#define COMPONENT_BIT_MODEL 0
		#define COMPONENT_BIT_ANIMATED 1
		#define COMPONENT_BIT_DYNAMIC 2
		#define COMPONENT_BIT_STATIC 3
		#define COMPONENT_BIT_STENCIL 4
		#define COMPONENT_BIT_CAM1 5
		#define COMPONENT_BIT_CAM2 6
		#define COMPONENT_BIT_COUNT 7
		std::bitset<COMPONENT_BIT_COUNT> m_signature;

		unsigned int m_id;
		std::string m_type;

		//std::unordered_map<unsigned int, void*> pointerBuffer; // make entity class into typless pointer container? <pointer_type_enum, void_casted_pointer>

		Transform* phystransform;
		Model* mdl;
		SkeletalModel* skMdl;
		Animator* mator;
		std::vector<Wire*> wires;
		btRigidBody* body;
		
};

#endif
