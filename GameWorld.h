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
        bool gameplayOver;
    public:
        GameWorld();
        Scene& getAsScene();
        //EntityManager entities;
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
        void draw(SceneDrawContext& context) override;
        void drawHUD(SceneDrawContext& context) const;
        SDL_Color backgroundColor;
    };
};