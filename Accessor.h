#pragma once
#include "PassKey.h"
#include <SDL.h>
#include <memory>
#include "Level.h"

namespace ssge {

    class WindowManager;
    class SceneManager;
    class Level;
    class EntityManager;
    class Entity;
    class Engine;
    class EntityReference;           // forward-declare the wrapper type
    enum class EntityClassID : int;  // forward-declare the enum class
    enum class SceneClassID : int;
    class InputManager;
    class Scene;
    class GameWorld;
    class MenuManager;

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
        void changeScene(SceneClassID sceneClassID);
        void goToLevel(int wantedLevel);
        void pause();
        void unpause();
        bool togglePause();
        bool isPaused() const;
        void setPause(bool pause);
    };

    class InputsAccess {
        InputManager* actual;
    public:
        explicit InputsAccess(InputManager* actual) : actual(actual) {}
        bool isPressed(int buttonIndex);
        bool isJustPressed(int buttonIndex);
        bool isJustReleased(int buttonIndex);
        uint32_t getCurrentButtonsForPlayer(int playerId);
    };
    
    class DrawingAccess {
        SDL_Renderer* renderer;
    public:
        explicit DrawingAccess(SDL_Renderer* renderer) : renderer(renderer) {}
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
        
        // Return tile indices overlapped by a rect (clamped to level bounds).
        void rectToTileSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const;

        // Query a tile (with OOB policy)
        Level::BlockQuery queryTile(int col, int row) const;

        // Axis-separated sweep: move horizontally by dx, collide with solids.
        Level::SweepHit sweepHorizontal(const SDL_FRect& rect, float dx) const;

        // Axis-separated sweep: move vertically by dy, collide with solids.
        Level::SweepHit sweepVertical(const SDL_FRect& rect, float dy) const;

        // Is any overlapped tile "water"?
        bool rectInWater(const SDL_FRect& r) const;
    };
    
    class EntitiesAccess {
        EntityManager* actual;
    public:
        explicit EntitiesAccess(EntityManager* actual) : actual(actual) {}
        //TBA
        //TODO: Spawning in new entities, entity lookup, etc.
        EntityReference addEntity(EntityClassID entityClassID);
    };

    class EntitiesAccessWCurrent : public EntitiesAccess {
    public:
        Entity& current;
        explicit EntitiesAccessWCurrent(EntityManager* actual, Entity* current) : EntitiesAccess(actual), current(*current) {}
    };

    class MenusAccess {
        MenuManager* actual;
    public:
        explicit MenusAccess(MenuManager* actual) : actual(actual) {}
        //TODO
        /*bool isOpen() const;
        void openMainMenu();
        void openPauseMenu();
        void close();*/
    };

} // namespace ssge
