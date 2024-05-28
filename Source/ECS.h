#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
#include "Model.h"
#include "Physics.h"
#include "SkeletalModel.h"
#include "Wire.h"
#include "Skeleton.h"
#include "Animator.h"
#include "Globals.h"
#include "Transform.h"

#include "btBulletDynamicsCommon.h"

#include <unordered_map>
#include <queue>

#define MAX_ENTITIES 65000

template <typename T> class ComponentSet {
    public:
        void linkEntity(unsigned int ID, T& val) {
            entSet.insert(ID);
            memArr[ID] = val;
        }
        void unlinkEntity(unsigned int ID) {
            entSet.erase(ID);
        }
        T getMem(unsigned int ID) {
            return memArr[ID];
        }
    private:
        std::unordered_set<unsigned int> entSet;
        T memArr[MAX_ENTITIES];
};

class ECS {
public:
    // defines an class operation that lets clients access its unique instance.
    static ECS& get() {
        // may be responsible for creating its own unique instance.
        if (nullptr == instance) instance = new ECS;
        return *instance;
    }
    ECS(const ECS&) = delete; // rule of three
    ECS& operator=(const ECS&) = delete;
    static void destruct() {
        delete instance;
        instance = nullptr;
    }

    unsigned int createEntity();

    void addModel(unsigned int ID, Model* mdl);
    void addPhysBody(unsigned int ID, btRigidBody* b);
    void addAnimator(unsigned int ID, Animator* mator);
    void addShader(unsigned int ID, Shader* sh);
    void addCamera(unsigned int ID, Camera* cam);
    void addTransform(unsigned int ID, Transform* transf);
    void addPhysTransform(unsigned int ID, Transform* phystransf);
    void addSkModel(unsigned int ID, SkeletalModel* skmdl);
    void addWire(unsigned int ID, Wire* w);
    void addWireFrame(unsigned int ID, float halfWidth, float halfHeight, float halfLength);

    void syncPhysics();
    void DrawEntityShadows();
    void advanceEntityAnimations(float delta);
    void DrawEntityPointShadows();
    void DrawEntityStencils();
    void DrawEntities();
    void deleteEntity(unsigned int ID);
    Entity getEntity(unsigned int ID);
    void updateEntity(Entity e);

    /*
    void registerComponents(Entity* e);
    void registerComponent(Entity* e, unsigned int bit);
    void unregisterComponent(Entity* e, unsigned int bit);
    */


private:
    ECS(); // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable

    // approx 4MB right here ...
    ComponentSet<Transform*> cset_transform;
    ComponentSet<Model*> cset_model;
    ComponentSet<SkeletalModel*> cset_skmodel;
    ComponentSet<Transform*> cset_phystransform;
    ComponentSet<Animator*> cset_animator;
    ComponentSet<Camera*> cset_camera;
    ComponentSet<Shader*> cset_shader;
    ComponentSet<btRigidBody*> cset_body;
    // no wire component set ideally

    std::queue<unsigned int> availableIDs;
    std::unordered_map<unsigned int, Entity> entMap;
};


#endif
