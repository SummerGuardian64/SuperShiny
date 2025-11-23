#pragma once
#include <string>
#include <memory>
#include "PassKey.h"
#include "SDL.h"
#include "Sprite.h"

namespace ssge
{
    class Engine;
    class StepContext;
    class DrawContext;
    class Scene;
    class ScenesAccess;
    class Entity;
    class EntitiesAccess;
    class Sprite;
    class GameAccess;
    class MenuCommandEx;
    class MenuContext;

    class IGameScenes;
    class IGameEntities;
    class IGameSprites;

	class IGame
	{
    public:
		virtual ~IGame() = default;

        // Called once after SDL + engine subsystems are up.
        virtual void init(StepContext& context) = 0;

        // Called every frame before SceneManager step
        virtual void step(StepContext& context) = 0;

        // Called after everything is drawn
        virtual void draw(DrawContext& context) = 0;

        // Called when it's time to clean up before SDL quits
        virtual void cleanUp(PassKey<Engine> pk) = 0;

        // Called upon shutdown to save settings
        virtual bool saveSettings(StepContext& context) = 0;

        // Tells the game implementation to save settings
        // TODO: Event system!!!
        virtual void saveSettings() = 0;

        // Tells the game implementation the game was won
        // TODO: Event system!!!
        virtual void declareVictory() = 0;

        // Tells the game it's time to quit
        // TODO: Event system!!!
        virtual void queryQuit() = 0;

        // Tells the game that a joypad got unplugged
        // TODO: Event system!!!
        virtual void joypadGotUnplugged() = 0;

        // Get application title
        virtual const char* getApplicationTitle() = 0;

        // Get virtual width
        virtual int getVirtualWidth() = 0;

        // Get virtual height
        virtual int getVirtualHeight() = 0;

        // Called by SceneManager Accessor to get the game's scene registry
        virtual IGameScenes& getScenes(PassKey<ScenesAccess> pk) = 0;

        // Called by EntityManager Accessor to get the game's entity registry
        virtual IGameEntities& getEntities(PassKey<EntitiesAccess> pk) = 0;

        // Called by parts of SSGE to get access to the sprites registry
        virtual IGameSprites& getSprites() = 0;

        // Called by the MenuManager when the user is backing out on the last item
        virtual MenuCommandEx onHavingBackedOutOfMenus(PassKey<GameAccess> pk, MenuContext& context) = 0;
    };

    // Gamedev's scene registry
    class IGameScenes
    {
    public:
        virtual ~IGameScenes() = default;

        // Called by SceneManager's Accessor to create a scene by ID
        virtual std::unique_ptr<Scene> createScene(PassKey<ScenesAccess> pk, std::string sceneId) = 0;

        // Tells the sceneID of the main menu scene
        virtual std::string getMainMenuSceneClassID() const = 0;
    };

    // Gamedev's entity registry
    class IGameEntities
    {
    public:
        virtual ~IGameEntities() = default;

        // Called by EntityManager's Accessor to create an entity by ID
        virtual std::shared_ptr<Entity> createEntity(PassKey<EntitiesAccess> pk, std::string entityId) = 0;
    };

    // Gamedev's sprite registry
    class IGameSprites
    {
    public:
        virtual ~IGameSprites() = default;

        // Called by initialization parts of SSGE to load a sprite definition's texture into the GPU via sprite definition ID
        virtual bool load(std::string sprdefId, SDL_Renderer* renderer) = 0;

        // Called by cleanup parts of SSGE to unload a sprite definition's texture from the GPU via sprite definition ID
        virtual void unload(std::string sprdefId) = 0;

        // Called by parts of SSGE that need to attach a sprite definition somewhere via sprite definition ID
        virtual const Sprite::Definition* fetchDefinition(const std::string& sprdefId) = 0;
    };
}
