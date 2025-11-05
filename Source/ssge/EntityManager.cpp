#include "EntityManager.h"
#include "StepContext.h"
#include "Game.h"
#include "PassKey.h"
#include <algorithm>

using namespace ssge;

void EntityManager::step(GameWorldStepContext& context)
{
    // Step all entities
    for (auto& entityPtr : entities)
    {
        EntityStepContext entityStepContext(
            PassKey<EntityManager>(),
            context.deltaTime,
            context.engine,
            context.scenes,
            context.inputs,
            context.drawing,
            context.currentScene,
            context.gameWorld,
            context.level,
            EntitiesAccessWCurrent(this, entityPtr.get())
        );

        entityPtr->latch(entityStepContext);
        entityPtr->step(entityStepContext);
    }

    destroyScheduledEntities(context);
}

void EntityManager::draw(DrawContext& context)
{
    for (auto& entity : entities)
    {
        // Don't draw an entity immediately! Give it time to initialize!
        if (entity->getLifespan() > 0)
        {
            DrawContext newContext = context.deriveForEntity(entity->position);
            entity->draw(newContext);
        }
    }
}

EntityCollection::iterator EntityManager::getEntitiesBegin()
{
    return entities.begin();
}

EntityCollection::iterator EntityManager::getEntitiesEnd()
{
    return entities.end();
}

EntityReference EntityManager::addEntity(EntityClassID id)
{
	auto entity = Game::Entities::create(id);
	if (!entity)
		return EntityReference(nullptr); // invalid ID

	entities.add(EntityAllocator(entity));
	return EntityReference(entity);
}

bool EntityManager::deleteEntity(EntityReference entity)
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
            EntityStepContext entityStepContext(
                PassKey<EntityManager>(),
                context.deltaTime,
                context.engine,
                context.scenes,
                context.inputs,
                context.drawing,
                context.currentScene,
                context.gameWorld,
                context.level,
                EntitiesAccessWCurrent(this, it->get())
            );
            (*it)->onDestroy(entityStepContext);
            it = entities.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

