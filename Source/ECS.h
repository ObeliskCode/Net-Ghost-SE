#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
#include <unordered_map>
#include <queue>

class ComponentSet {
    public:
        void linkEntity(Entity* e) {
            componentMap[e->getID()] = e;
        }
        void unlinkEntity(unsigned int ID) {
            componentMap.erase(ID);
        }
        const auto& getMap() {
            return componentMap;
        }
    private:
        std::unordered_map<unsigned int, Entity*> componentMap; // possible make <unsigned int, std::pair<Entity*, Component>> for seperate component memory
        // also make system for grouping entities like ComponentSet does orignally?
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

    Entity* linkEntity(Entity* e);
    void updatePhysics();
    void DrawEntityShadows();
    void advanceEntityAnimations(float delta);
    void DrawEntityPointShadows();
    void DrawEntityStencils();
    void DrawEntities();
    void deleteEntity(unsigned int ID);
    Entity* getEntity(unsigned int ID);

    void registerComponents(Entity* e);
    void registerComponent(Entity* e, unsigned int bit);
    void unregisterComponent(Entity* e, unsigned int bit);

private:
    ECS(); // no public constructor
    ~ECS(); // no public destructor
    static ECS* instance; // declaration class variable

    ComponentSet componentSets[COMPONENT_BIT_COUNT];

    std::queue<unsigned int> availableIDs;
    std::unordered_map<unsigned int, Entity*> entMap;
};

#define MAX_ENTITIES 5000

#endif