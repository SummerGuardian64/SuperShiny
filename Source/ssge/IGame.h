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

        // Get application title
        virtual const char* getApplicationTitle() = 0;

        virtual int getVirtualWidth() = 0;

        virtual int getVirtualHeight() = 0;

        virtual IGameScenes& getScenes(PassKey<ScenesAccess> pk) = 0;

        virtual IGameEntities& getEntities(PassKey<EntitiesAccess> pk) = 0;

        virtual IGameSprites& getSprites() = 0;
    };

    class IGameScenes
    {
    public:
        virtual ~IGameScenes() = default;

        virtual std::unique_ptr<Scene> createScene(PassKey<ScenesAccess> pk, std::string sceneId) = 0;
    };

    class IGameEntities
    {
    public:
        virtual ~IGameEntities() = default;

        virtual std::shared_ptr<Entity> createEntity(PassKey<EntitiesAccess> pk, std::string entityId) = 0;
    };

    class IGameSprites
    {
    public:
        virtual ~IGameSprites() = default;

        virtual bool load(std::string sprdefId, SDL_Renderer* renderer) = 0;
        virtual void unload(std::string sprdefId) = 0;
        virtual const Sprite::Definition* fetchDefinition(const std::string& sprdefId) = 0;
    };
}