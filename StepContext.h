#pragma once
#include <memory>
#include "SDL.h"
#include "PassKey.h"
#include "Accessor.h"

namespace ssge {
    // Engine's systems

    class Engine;
    class WindowManager;
    class InputManager;
    class SceneManager;
    class Scene;
    class GameWorld;
    class EntityManager;
    
    class StepContextBase {
    public:
        // Deltatime
        const double deltaTime{ 0.0 };
        StepContextBase(double deltaTime);
    };

    class StepContext : public StepContextBase {
    public:
        EngineAccess engine;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;

        explicit StepContext(
            PassKey<Engine> pk,
            double deltaTime,
            EngineAccess engine,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing
        );
    };

    class SceneStepContext : public StepContextBase {
    public:
        EngineAccess engine;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        CurrentSceneAccess currentScene;

        explicit SceneStepContext(
            PassKey<SceneManager> pk,
            double deltaTime,
            EngineAccess engine,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            CurrentSceneAccess currentScene
        );
    };

    class GameWorldStepContext : public StepContextBase {
    public:
        EngineAccess engine;
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
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level
        );
    };


    class EntityStepContext : public StepContextBase {
    public:
        EngineAccess engine;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        CurrentSceneAccess currentScene;
        GameWorldAccess gameWorld;
        LevelAccess level;
        EntitiesAccessWCurrent entities;

        explicit EntityStepContext(
            PassKey<EntityManager> pk,
            double deltaTime,
            EngineAccess engine,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level,
            EntitiesAccessWCurrent entitiesAndCurrent
        );
    };

} // namespace ssge
