#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
#include <unordered_map>
#include <queue>

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

    Entity* linkEntity(Entity* e);
    void updatePhysics();
    void DrawEntityShadows(float delta);
    void DrawEntityStencils();
    void DrawEntities();
    void deleteEntity(unsigned int ID);
    Entity* getEntity(unsigned int ID);

private:
    ECS(); // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable

    std::queue<unsigned int> availableIDs;
    std::unordered_map<unsigned int, Entity*> entMap;
};

#define MAX_ENTITIES 5000

#endif