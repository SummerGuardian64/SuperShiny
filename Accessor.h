#pragma once
#include "PassKey.h"
#include <SDL.h>
#include <memory>

namespace ssge {

    class WindowManager;
    class SceneManager;
    class Level;
    class EntityManager;
    class Engine;
    enum class SceneClassID : int;
    class InputManager;
    class Scene;
    class GameWorld;

    class EngineAccess {
        Engine* actual;
    public:
        explicit EngineAccess(Engine* actual) : actual(actual) {};
        void finish();
        void wrapUp();
    };

    class ScenesAccess {
        SceneManager* actual;
    public:
        explicit ScenesAccess(SceneManager* actual) : actual(actual) {}
        void changeScene(SceneClassID newScene);
        void pause(bool pause = true);
        void unpause();
        bool isPaused();
    };

    class InputsAccess {
        InputManager* actual;
    public:
        explicit InputsAccess(InputManager* actual) : actual(actual) {}
        bool isPressed(int buttonIndex);
        bool isJustPressed(int buttonIndex);
        bool isJustReleased(int buttonIndex);
    };
    
    class DrawingAccess {
        SDL_Renderer* renderer;
    public:
        explicit DrawingAccess(SDL_Renderer* r) : renderer(renderer) {}
        SDL_Renderer* getRenderer() const { return renderer; }
        void fillRect(const SDL_Rect& rect, SDL_Color color) const {
            if (!renderer) return;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    };

    class CurrentSceneAccess {
        Scene* currentScene;
    public:
        explicit CurrentSceneAccess(Scene* current) : currentScene(current) {}
        SceneClassID getSceneClassID() const;
    };

    class GameWorldAccess {
        GameWorld* currentGameWorld;
    public:
        explicit GameWorldAccess(GameWorld* current) : currentGameWorld(current) {}
        //TBA
    };

    class LevelAccess {
        Level* actual;
    public:
        explicit LevelAccess(Level* actual) : actual(actual) {}

        bool valid() const;
        bool rectOverlapsSolid(const SDL_FRect& r) const;  // implemented to forward to Level
        //auto sweepAABB(const SDL_FRect& box, SDL_FPoint d) const; // forward to Level
    };
    
    class EntitiesAccess {
        EntityManager* actual;
    public:
        explicit EntitiesAccess(EntityManager* actual) : actual(actual) {}
        //TBA
        //TODO: Spawning in new entities, entity lookup, etc.
    };

} // namespace ssge
