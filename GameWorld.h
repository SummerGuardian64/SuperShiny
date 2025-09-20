#pragma once
#include "SuperShiny.h"
//#include "Scene.h"
#include <vector>
//#include "Entity.h"
#include <memory>
//#include "Level.h"
//#include "LevelLoader.h"
//#include "PowerupType.h"
#include <list>
//#include "HintManager.h"

// Forward declarations
//class Entity;
//class Game;
//class Ball;
//enum class EntityClassID;

namespace ssge
{
    class GameWorld : public Scene
    {
        SDL_FRect confines;
        //std::vector<std::unique_ptr<PowerupType>> powerupTypes;
        //void initPowerupTypes();
        bool gameplayOver;
    public:
        GameWorld();
        //std::unique_ptr<Level> level;
        //HintManager hints;
        bool isGameplayOver() const;
        void finishGameplay();
        //std::list<std::shared_ptr<Entity>> entities;
        //std::list<std::shared_ptr<Entity>>::iterator getEntitiesBegin();
        //std::list<std::shared_ptr<Entity>>::iterator getEntitiesEnd();
        //std::weak_ptr<Entity> addEntity(std::shared_ptr<Entity> entity);
        //bool deleteEntity(std::shared_ptr<Entity> entity);
        //Entity* findEntity(EntityClassID entityClassID);
        //const Entity* findConstEntity(EntityClassID entityClassID) const;
        //std::vector<std::weak_ptr<Entity>> findAllEntities(EntityClassID entityClassID);
        //int countAllEntities() const;
        //int countAllEntities(EntityClassID entityClassID) const;
        //void destroyScheduledEntities();
        //PowerupType* fetchPowerupType(PowerupTypeID powerupTypeID);
        //void checkBrickCollision(Entity& entity);
        SDL_FRect getConstConfines() const;
        SDL_FRect getConfines();
        void setConfines(SDL_FRect confines);
        //sf::Vector2i getMousePosition();
        //std::weak_ptr<Ball> spawnNewBall();
        SceneClassID getSceneClassID() const override;
        void init(SceneStepContext& context) override;
        void step(SceneStepContext& context) override;
        void draw(SceneDrawContext& context) override;
        void drawHUD(SceneDrawContext& context) const;
        SDL_Color backgroundColor;
    };
};