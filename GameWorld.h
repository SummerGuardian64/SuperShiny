#pragma once
#include "Scene.h"
#include "EntityManager.h"
#include "SDL.h"
#include "StepContext.h"
#include <memory>
#include <vector>
#include <list>
#include "Level.h"
//#include "LevelLoader.h"
//#include "PowerupType.h"
//#include "HintManager.h"
static_assert(sizeof(ssge::Scene) > 0, "Scene must be defined");
static_assert(sizeof(ssge::EntityManager) > 0, "EntityManager must be defined");

namespace ssge
{
    class GameWorld : public Scene
    {
        SDL_FRect confines;
        bool gameplayOver;
        int wantedLevel;
        bool initLevel(SceneStepContext& context);
        Level::Loader levelLoader;
    public:
        GameWorld();
        GameWorld(int wantedLevel);
        Scene& getAsScene();
        EntityManager entities;
        std::unique_ptr<Level> level;
        EntityReference entityToScrollTo;
        SDL_FPoint scrollTarget;
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