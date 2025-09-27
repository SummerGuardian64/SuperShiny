#pragma once
#include "SuperShiny.h"
#include <memory>

namespace ssge {
    // Engine types

    class SceneManager;
    class Scene;
    class GameWorld;
    
    // Forward context classes
    
    class SceneStepContext;
    class GameWorldStepContext;
    class EntityStepContext;

    class StepContext {
    public:
        class Engine {
            explicit Engine(ssge::Engine* actual);
            ssge::Engine* actual = nullptr;
            friend class StepContext;
        public:
            void finish();
        };

        class Scenes {
            explicit Scenes(ssge::SceneManager* actual);
            ssge::SceneManager* actual = nullptr;
            friend class StepContext;
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);
        };

        class Inputs {
            explicit Inputs(ssge::InputManager* actual);
            ssge::InputManager* actual = nullptr;
            friend class StepContext;
        public:
            //bool isPressed();
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;

    private:
        StepContext(ssge::Engine* actualEngine);

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();

        // Strict lifetime rules: no copying allowed.
        StepContext(const StepContext&) = delete;
        StepContext& operator=(const StepContext&) = delete;

    public:
        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        StepContext(StepContext&&) = default;
        StepContext& operator=(StepContext&&) = delete;

        // Trusted classes:
        friend class Factory;
        friend class ::ssge::SceneStepContext; // For copying information into
    };
    
    class SceneStepContext {
    public:
        class Engine {
            explicit Engine(ssge::Engine* actual);
            ssge::Engine* actual = nullptr;
            friend class SceneStepContext;
        public:
            void finish();
        };

        class Scenes {
            explicit Scenes(ssge::SceneManager* actual);
            ssge::SceneManager* actual = nullptr;
            friend class SceneStepContext;
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);
        };

        class Inputs {
            explicit Inputs(ssge::InputManager* actual);
            ssge::InputManager* actual = nullptr;
            friend class SceneStepContext;
        public:
            //bool isPressed();
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Scene& currentScene;

    private:
        SceneStepContext(ssge::Scene& currentScene, ssge::StepContext& stepContext);

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();

        // Strict lifetime rules: no copying allowed.
        SceneStepContext(const SceneStepContext&) = delete;
        SceneStepContext& operator=(const SceneStepContext&) = delete;

    public:
        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        SceneStepContext(SceneStepContext&&) = default;
        SceneStepContext& operator=(SceneStepContext&&) = delete;

        // Trusted classes:
        friend class Factory;
        friend class ::ssge::GameWorldStepContext; // For initialization
    };

    class GameWorldStepContext {
    public:
        class Engine {
            explicit Engine(ssge::Engine* actual);
            ssge::Engine* actual = nullptr;
            friend class GameWorldStepContext;
        public:
            void finish();
        };

        class Scenes {
            explicit Scenes(ssge::SceneManager* actual);
            ssge::SceneManager* actual = nullptr;
            friend class GameWorldStepContext;
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);
        };

        class Inputs {
            explicit Inputs(ssge::InputManager* actual);
            ssge::InputManager* actual = nullptr;
            friend class GameWorldStepContext;
        public:
            //bool isPressed();
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Scene& currentScene;
        GameWorld& world;

    private:
        GameWorldStepContext(ssge::GameWorld& currentGameWorld, ssge::SceneStepContext& sceneStepContext);

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();

        // Strict lifetime rules: no copying allowed.
        GameWorldStepContext(const GameWorldStepContext&) = delete;
        GameWorldStepContext& operator=(const GameWorldStepContext&) = delete;

    public:
        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        GameWorldStepContext(GameWorldStepContext&&) = default;
        GameWorldStepContext& operator=(GameWorldStepContext&&) = delete;

        // Trusted classes:
        friend class Factory;
    };

} // namespace ssge
