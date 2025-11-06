#pragma once
#include <memory>
#include "SDL.h"
#include "PassKey.h"
#include "Accessor.h"

namespace ssge {
    // Engine's systems

    class Engine;
    class IGame;
    class WindowManager;
    class InputManager;
    class SceneManager;
    class Scene;
    class GameWorld;
    class EntityManager;
    class MenuManager;
    
    class StepContextBase
    {
    public:
        // Deltatime
        const double deltaTime{ 0.0 };
        StepContextBase(double deltaTime);
    };

    class StepContext : public StepContextBase
    {
    public:
        EngineAccess engine;
        GameAccess game;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        MenusAccess menus;

        explicit StepContext(
            PassKey<Engine> pk,
            double deltaTime,
            EngineAccess engine,
            GameAccess game,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            MenusAccess menus
        );
    };

    class SceneStepContext : public StepContextBase
    {
    public:
        EngineAccess engine;
        GameAccess game;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        MenusAccess menus;
        CurrentSceneAccess currentScene;

        explicit SceneStepContext(
            PassKey<SceneManager> pk,
            double deltaTime,
            EngineAccess engine,
            GameAccess game,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            MenusAccess menus,
            CurrentSceneAccess currentScene
        );
    };

    class GameWorldStepContext : public StepContextBase
    {
    public:
        EngineAccess engine;
        GameAccess game;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        CurrentSceneAccess currentScene;
        GameWorldAccess gameWorld;
        LevelAccess level;

        explicit GameWorldStepContext(
            PassKey<GameWorld> pk,
            double deltaTime,
            EngineAccess engine,
            GameAccess game,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level
        );
    };


    class EntityStepContext : public StepContextBase
    {
    public:
        EngineAccess engine;
        GameAccess game;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        CurrentSceneAccess currentScene;
        GameWorldAccess gameWorld;
        LevelAccess level;
        EntitiesAccessWCurrent entities;
        SpritesAccess sprites;

        explicit EntityStepContext(
            PassKey<EntityManager> pk,
            double deltaTime,
            EngineAccess engine,
            GameAccess game,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level,
            EntitiesAccessWCurrent entitiesAndCurrent,
            SpritesAccess sprites
        );
    };

} // namespace ssge
