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
#include <vector>

#define MAX_ENTITIES 65000

template <typename T> class ComponentMemory {
    public:
        // never link an Entity Twice!
        void linkEntity(unsigned int ID, T val) {
            entVec.push_back(ID);
            memArr[ID] = val;
        }
        void unlinkEntity(unsigned int ID) {
            for (auto it = entVec.begin(); it != entVec.end(); it++ ) {
                if (*it == ID){
                    entVec.erase(it);
                    break;
                }
            }
        }
        T& getMem(unsigned int ID) {
            return memArr[ID];
        }
        std::vector<unsigned int> entVec;
    private:
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
    void addShader(unsigned int ID, Shader sh);
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
    void DrawScreenEntity(unsigned int ID);
    void DrawEntity(unsigned int ID);

    void deleteEntity(unsigned int ID);
    Entity getEntity(unsigned int ID);
    void updateEntity(Entity e);

    // approx 4MB right here ...
    ComponentMemory<Transform*> cset_transform; // can transform and transform be passed as stack allocated objects?
    ComponentMemory<Model*> cset_model;
    ComponentMemory<SkeletalModel*> cset_skmodel;
    ComponentMemory<Transform*> cset_phystransform;
    ComponentMemory<Animator*> cset_animator;
    ComponentMemory<Camera*> cset_camera; // do we need this many camera pointers // only 2 cams rn
    ComponentMemory<Shader> cset_shader;
    ComponentMemory<btRigidBody*> cset_body;
    ComponentMemory<std::vector<Wire*>> cset_wire;
    // no wire component set ideally

    std::unordered_set<unsigned int> stencilSet;

private:
    ECS(); // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable


    std::unordered_set<unsigned int> cam1Set;
    std::unordered_set<unsigned int> cam2Set;

    Globals& globals = Globals::get();

    std::queue<unsigned int> availableIDs;
    std::unordered_map<unsigned int, Entity> entMap;
    std::vector<unsigned int> entVec;
};


#endif
