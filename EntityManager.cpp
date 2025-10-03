#include "EntityManager.h"

using namespace ssge;

void ssge::EntityManager::step(GameWorldStepContext& context)
{
    // Step all entities
    for (auto& entityPtr : entities)
    {
        EntityStepContext entityStepContext = Factory::ForEntityManager::entityStepContext(context);
        entityPtr->step(entityStepContext);
    }

    destroyScheduledEntities(context);
}

void ssge::EntityManager::draw(DrawContext& context)
{

}

EntityCollection::iterator EntityManager::getEntitiesBegin()
{
    return entities.begin();
}

EntityCollection::iterator EntityManager::getEntitiesEnd()
{
    return entities.end();
}

EntityReference EntityManager::addEntity(EntityAllocator entity)
{
    if (entity)
    {
        entities.push_back(entity);
        return entity;
    }
    else
    {
        return EntityReference(); // Return an empty weak_ptr
    }
}

bool EntityManager::deleteEntity(EntityAllocator entity)
{
    auto it = std::find(entities.begin(), entities.end(), entity);

    if (it != entities.end())
    {
        // Schedule the entity to get destroyed
        (*it)->destroy();
        return true;
    }
    else
    {
        return false;
    }
}

Entity* EntityManager::findEntity(EntityClassID entityClassID)
{
    for (auto& entity : entities)
    {
        if (entity->isScheduledToDestroy())
        { // Don't count entities that are scheduled to be destroyed
            continue;
        }
        if (entity->getEntityClassID() == entityClassID)
        {
            return entity.get();
        }
    }
    return nullptr;
}

const Entity* EntityManager::findConstEntity(EntityClassID entityClassID) const
{
    for (auto& entity : entities)
    {
        if (entity->isScheduledToDestroy())
        { // Don't count entities that are scheduled to be destroyed
            continue;
        }
        if (entity->getEntityClassID() == entityClassID)
        {
            return entity.get();
        }
    }
    return nullptr;
}

EntityQueryResult EntityManager::findAllEntities(EntityClassID entityClassID)
{
    EntityQueryResult foundEntities;

    for (auto& entity : entities)
    {
        if (entity->isScheduledToDestroy())
        { // Don't count entities that are scheduled to be destroyed
            continue;
        }
        if (entity->getEntityClassID() == entityClassID)
        {
            foundEntities.push_back(entity);
        }
    }
    return foundEntities;
}

int EntityManager::countAllEntities() const
{
    int count = 0;

    for (auto& entity : entities)
    {
        if (entity->isScheduledToDestroy())
        { // Don't count entities that are scheduled to be destroyed
            continue;
        }
        count++;
    }
    return count;
}

int EntityManager::countAllEntities(EntityClassID entityClassID) const
{
    int count = 0;

    for (auto& entity : entities)
    {
        if (entity->isScheduledToDestroy())
        { // Don't count entities that are scheduled to be destroyed
            continue;
        }
        if (entity->getEntityClassID() == entityClassID)
        {
            count++;
        }
    }
    return count;
}

void EntityManager::destroyScheduledEntities(GameWorldStepContext& context)
{
    for (auto it = entities.begin(); it != entities.end(); )
    {
        if ((*it)->isScheduledToDestroy())
        {
            EntityStepContext entityStepContext = Factory::ForEntityManager::entityStepContext(context);
            (*it)->onDestroy(entityStepContext);
            it = entities.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

