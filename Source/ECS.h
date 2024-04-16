#ifndef ECS_H
#define ECS_H

#include "Entity.h"
#include "Physics.h"
#include <unordered_map>
#include <queue>

// Conjective Theory
// unraveling of conjectives, conjective function means combinatorially complex boolean statement.
// unraveling them means transformations break the continuity of the entire conjective. //SAT is a conjective
// do not unravel/combine SATs instead compose them and transform to a different space
// For example of a conjecture function is a transform matrix because it does not require a special matrix
// h(x) = T1[x1] + T2[x2], o(T) = T', oh(x) = o(T1[x1] + T2[x2])

//imagine simulating our reality
//you cannot simulate our infinity
//yet you may simulate a smaller infinity
//inf^ x = inf; x != 0?
//contradictively
//yet both facts are true
//how is this possible

//what is 2m in a smaller simulated infinity, ... 2m

//within a point there is everything
//that is why we have no area to them
//we have 0 space to them yet infinitely much space to us
//everything is within a point

//i may be wrong on my corollary
//inf^ x = inf; x is real
//i believe that inf ^ 0 = inf
//inf ^ 0 = inf explains infinite space in a point!oblivion!
//inf ^ 0 = inf should be pushed!!

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
    void DrawEntityShadows(float delta);
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