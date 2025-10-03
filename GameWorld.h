#pragma once
#include "Scene.h"
#include "SDL.h"
#include <vector>
#include "StepContext.h"
#include "EntityManager.h"
//#include "Entity.h"
#include <memory>
//#include "Level.h"
//#include "LevelLoader.h"
//#include "PowerupType.h"
#include <list>
//#include "HintManager.h"
static_assert(sizeof(ssge::Scene) > 0, "Scene must be defined");
static_assert(sizeof(ssge::EntityManager) > 0, "EntityManager must be defined");


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
        bool gameplayOver;
    public:
        GameWorld();
        Scene& getAsScene();
        ssge::EntityManager entities;
        //std::unique_ptr<Level> level;
        //HintManager hints;
        bool isGameplayOver() const;
        void finishGameplay();
        SDL_FRect getConstConfines() const;
        SDL_FRect getConfines();
        void setConfines(SDL_FRect confines);
        //sf::Vector2i getMousePosition();
        //std::weak_ptr<Ball> spawnNewBall();
        SceneClassID getSceneClassID() const override;
        void init(SceneStepContext& context) override;
        void step(SceneStepContext& context) override;
        void draw(DrawContext& context) override;
        void drawHUD(DrawContext& context) const;
        SDL_Color backgroundColor;
    };
};