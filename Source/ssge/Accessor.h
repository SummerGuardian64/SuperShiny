#pragma once
#include "PassKey.h"
#include <SDL.h>
#include <memory>
#include "Level.h"
#include "IGame.h"

namespace ssge {

    class WindowManager;
    class SceneManager;
    class Level;
    class EntityManager;
    class Entity;
    class Engine;
    class EntityReference;
    class InputManager;
    class Scene;
    class GameWorld;
    class MenuManager;
    class MenuCommandEx;
    class MenuContext;
    class MenuHeader;

    class EngineAccess {
        Engine* actual;
    public:
        explicit EngineAccess(Engine* actual) : actual(actual) {};
        void finish();
        void wrapUp();
        bool isWrappingUp() const;
    };

    class GameAccess {
        IGame& actual;
    public:
        explicit GameAccess(IGame& actual) : actual(actual) {}
        IGame& get() { return actual; }
        MenuCommandEx onHavingBackedOutOfMenus(PassKey<MenuManager> pk, MenuContext& context);
    };

    class ScenesAccess {
        SceneManager* actual;
        IGameScenes& gameScenes;
    public:
        explicit ScenesAccess(SceneManager* actual, IGame& game)
            : actual(actual), gameScenes(game.getScenes(PassKey<ScenesAccess>())) { }
        void changeScene(std::string newSceneId);
        std::string getCurrentSceneClassID() const;
        void goToLevel(int wantedLevel);
        void goToMainMenu();
        void pause();
        void unpause();
        bool togglePause();
        bool isPaused() const;
        void setPause(bool pause);
        bool isFadeFinished() const;
        void restart();
    };

    class WindowAccess {
        WindowManager* actual;
    public:
        explicit WindowAccess(WindowManager* actual)
            : actual(actual) {}
        // Gets the window
        SDL_Window* getWindow() const;
        // Gets the window surface
        SDL_Surface* getWindowSurface() const;
        // Gets the renderer for the window
        SDL_Renderer* getRenderer() const;
        // Gets virtual width
        int getVirtualWidth() const;
        // Gets virtual height
        int getVirtualHeight() const;
        // Returns true if upscale is integral, false if fractional
        bool isUpscaleIntegral() const;
        // Sets integral upscale, providing false sets fractional upscale
        void setIntegralUpscale(bool integralUpscale = true);
        // Sets bordererd fullscreen or windowed mode
        void setBorderedFullScreen(bool borderedFullScreen = false);
        // Makes a SDL_Rect with best scaling
        SDL_Rect makeBestFitScale() const;
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
        std::string getSceneClassID() const;
    };

    class GameWorldAccess {
        GameWorld* currentGameWorld;
    public:
        explicit GameWorldAccess(GameWorld* current) : currentGameWorld(current) {}
        void reportHeroDeadth();
        //TBA
    };

    class LevelAccess {
        Level* actual;
    public:
        explicit LevelAccess(Level* actual) : actual(actual) {}

        bool valid() const;
        bool rectOverlapsSolid(const SDL_FRect& r) const;  // implemented to forward to Level
        
        // Return tile indices overlapped by a rect (clamped to level bounds).
        void rectToBlockSpan(const SDL_FRect& r, int& col0, int& col1, int& row0, int& row1) const;

        Level::Block* getBlockAt(Level::Block::Coords coords);

        const Level::Block* getConstBlockAt(Level::Block::Coords coords) const;

        SDL_Rect calculateLevelSize() const;

        // Query a block (with OOB policy)
        Level::BlockQuery queryBlock(int col, int row) const;

        // Query a block at specific position in level
        Level::BlockQuery queryBlock(SDL_FPoint positionInLevel) const;

        // Queries all blocks that overlap the specified collider
        std::vector<Level::BlockQuery> queryBlocksUnderCollider(SDL_FRect collider) const;

        // Axis-separated sweep: move horizontally by dx, collide with solids.
        Level::SweepHit sweepHorizontal(const SDL_FRect& rect, float dx) const;

        // Axis-separated sweep: move vertically by dy, collide with solids.
        Level::SweepHit sweepVertical(const SDL_FRect& rect, float dy) const;

        // Is any overlapped tile "water"?
        bool rectInWater(const SDL_FRect& r) const;
    };
    
    class EntitiesAccess {
        EntityManager* actual;
        IGameEntities& gameEntities;
    public:
        explicit EntitiesAccess(EntityManager* actual, GameAccess& game)
            : actual(actual), gameEntities(game.get().getEntities(PassKey<EntitiesAccess>())) { }
        //TBA
        //TODO: Spawning in new entities, entity lookup, etc.
        EntityReference addEntity(std::string entityID);
    };

    class EntitiesAccessWCurrent : public EntitiesAccess {
    public:
        Entity& current;
        explicit EntitiesAccessWCurrent(EntityManager* actual, GameAccess& game, Entity* current)
            : EntitiesAccess(actual, game), current(*current) {}
    };
    
    class SpritesAccess {
        IGameSprites& actual;
    public:
        explicit SpritesAccess(IGameSprites& actual) : actual(actual) {}
        const Sprite::Definition* fetchDefinition(std::string sprdefId);
        std::unique_ptr<Sprite> create(std::string sprdefId);
    };

    class MenusAccess {
        MenuManager* actual;
    public:
        explicit MenusAccess() : actual(nullptr) {}; // TODO: Remove this later!
        explicit MenusAccess(MenuManager* actual) : actual(actual) {}
        bool isOpen() const;
        void setMenu(MenuHeader& menuHeader);
        void setMenu(MenuHeader* menuHeader);
        void openMainMenu();
        void openPauseMenu();
        void close();
    };

} // namespace ssge
