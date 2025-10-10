#pragma once
#include <memory>
#include "SDL.h"
#include "PassKey.h"

namespace ssge {
    // Engine types

    class Engine;
    class SceneManager;
    class Scene;
    class GameWorld;
    class EntityManager;
    class WindowManager;
    class InputManager;
    
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
            void wrapUp();
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
            bool isPressed(int buttonIndex);
            bool isJustPressed(int buttonIndex);
            bool isJustReleased(int buttonIndex);
        };

        class Drawing {
            explicit Drawing(ssge::WindowManager* windowManager);
            ssge::WindowManager* windowManager = nullptr;
            friend class StepContext;
        public:
            SDL_Renderer* getRenderer() const;
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Drawing drawing;

        double getDeltaTime();

    public:
        StepContext(ssge::PassKey<ssge::Engine> pk, ssge::Engine* actualEngine, double deltaTime);
    private:

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();
        ssge::WindowManager* getActualWindowManager();

        double deltaTime;

        // Strict lifetime rules: no copying allowed.
        StepContext(const StepContext&) = delete;
        StepContext& operator=(const StepContext&) = delete;

        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        StepContext(StepContext&&) noexcept = default;
        StepContext& operator=(StepContext&&) = delete;

        // Trusted classes:
        //TODO: Do something about this!
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
            void wrapUp();
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
            bool isPressed(int buttonIndex);
            bool isJustPressed(int buttonIndex);
            bool isJustReleased(int buttonIndex);
        };

        class Drawing {
            explicit Drawing(ssge::WindowManager* windowManager);
            ssge::WindowManager* windowManager = nullptr;
            friend class SceneStepContext;
        public:
            SDL_Renderer* getRenderer() const;
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Drawing drawing;
        Scene& currentScene;

        double getDeltaTime();

    public:
        SceneStepContext(ssge::PassKey<SceneManager> pk, ssge::Scene& currentScene, ssge::StepContext& stepContext);
    private:
        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();
        ssge::WindowManager* getActualWindowManager();

        double deltaTime;

        // Strict lifetime rules: no copying allowed.
        SceneStepContext(const SceneStepContext&) = delete;
        SceneStepContext& operator=(const SceneStepContext&) = delete;

        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        SceneStepContext(SceneStepContext&&) noexcept = default;
        SceneStepContext& operator=(SceneStepContext&&) = delete;

        // Trusted classes:
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
            void wrapUp();
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
            bool isPressed(int buttonIndex);
            bool isJustPressed(int buttonIndex);
            bool isJustReleased(int buttonIndex);
        };

        class Drawing {
            explicit Drawing(ssge::WindowManager* windowManager);
            ssge::WindowManager* windowManager = nullptr;
            friend class GameWorldStepContext;
        public:
            SDL_Renderer* getRenderer() const;
        };

        class Scene
        {
            explicit Scene(ssge::Scene* actual);
            ssge::Scene* actual = nullptr;
            friend class GameWorldStepContext;
        public:
            //TBA
        };

        class GameWorld
        {
            explicit GameWorld(ssge::GameWorld* actual);
            ssge::GameWorld* actual = nullptr;
            friend class GameWorldStepContext;
        public:
            //TBA
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Drawing drawing;
        Scene currentScene;
        GameWorld world;

        double getDeltaTime();

    private:
        GameWorldStepContext(ssge::GameWorld& currentGameWorld, ssge::SceneStepContext& sceneStepContext);

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();
        ssge::WindowManager* getActualWindowManager();
        ssge::Scene* getCurrentScene();
        ssge::GameWorld* getCurrentGameWorld();

        double deltaTime;

        // Strict lifetime rules: no copying allowed.
        GameWorldStepContext(const GameWorldStepContext&) = delete;
        GameWorldStepContext& operator=(const GameWorldStepContext&) = delete;

        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        GameWorldStepContext(GameWorldStepContext&&) noexcept = default;
        GameWorldStepContext& operator=(GameWorldStepContext&&) = delete;

        // Trusted classes:
        friend class Factory;
        friend class ::ssge::EntityStepContext; // For initialization
    };

    class EntityStepContext {
    public:
        class Engine {
            explicit Engine(ssge::Engine* actual);
            ssge::Engine* actual = nullptr;
            friend class EntityStepContext;
        public:
            void finish();
            void wrapUp();
        };

        class Scenes {
            explicit Scenes(ssge::SceneManager* actual);
            ssge::SceneManager* actual = nullptr;
            friend class EntityStepContext;
        public:
            void changeScene(std::unique_ptr<ssge::Scene> newScene);
        };

        class Inputs {
            explicit Inputs(ssge::InputManager* actual);
            ssge::InputManager* actual = nullptr;
            friend class EntityStepContext;
        public:
            bool isPressed(int buttonIndex);
            bool isJustPressed(int buttonIndex);
            bool isJustReleased(int buttonIndex);
        };

        class Drawing {
            explicit Drawing(ssge::WindowManager* windowManager);
            ssge::WindowManager* windowManager = nullptr;
            friend class EntityStepContext;
        public:
            SDL_Renderer* getRenderer() const;
        };

        class Scene
        {
            explicit Scene(ssge::Scene* actual);
            ssge::Scene* actual = nullptr;
            friend class EntityStepContext;
        public:
            //TBA
        };

        class GameWorld
        {
            explicit GameWorld(ssge::GameWorld* actual);
            ssge::GameWorld* actual = nullptr;
            friend class EntityStepContext;
        public:
            //TBA
        };

        class EntityManager
        {
            explicit EntityManager(ssge::EntityManager* actual);
            ssge::EntityManager* actual = nullptr;
            friend class EntityStepContext;
        public:
            //TBA
        };

        // public sugar
        Engine engine;
        Scenes scenes;
        Inputs inputs;
        Drawing drawing;
        Scene currentScene;
        GameWorld world;
        EntityManager entities;

        double getDeltaTime();

    private:
        EntityStepContext(ssge::GameWorldStepContext& gameWorldStepContext);

        ssge::Engine* getActualEngine();
        ssge::SceneManager* getActualSceneManager();
        ssge::InputManager* getActualInputManager();
        ssge::WindowManager* getActualWindowManager();

        double deltaTime;

        // Strict lifetime rules: no copying allowed.
        EntityStepContext(const EntityStepContext&) = delete;
        EntityStepContext& operator=(const EntityStepContext&) = delete;

        // Move exists but is private so only friends can move it into locals if needed (return-from-factory).
        EntityStepContext(EntityStepContext&&) noexcept = default;
        EntityStepContext& operator=(EntityStepContext&&) = delete;

        // Trusted classes:
        friend class Factory;
    };

} // namespace ssge
