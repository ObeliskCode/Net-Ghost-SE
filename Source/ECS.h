#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
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
    private:
        std::unordered_set<unsigned int> entSet;
        T[MAX_ENTITIES] memArr;
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

    void updatePhysics();
    void DrawEntityShadows();
    void advanceEntityAnimations(float delta);
    void DrawEntityPointShadows();
    void DrawEntityStencils();
    void DrawEntities();
    void deleteEntity(unsigned int ID);
    Entity getEntity(unsigned int ID);

    /*
    void registerComponents(Entity* e);
    void registerComponent(Entity* e, unsigned int bit);
    void unregisterComponent(Entity* e, unsigned int bit);
    */


private:
    ECS(); // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable

    ComponentSet<Transform> cset_transform;

    std::queue<unsigned int> availableIDs;
    std::unordered_map<unsigned int, Entity> entMap;
};


#endif
