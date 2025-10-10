#pragma once

namespace ssge
{
	class Program;
	class Engine;
	class Scene;
	class SceneManager;
	class GameWorld;
	class EntityManager;
	class StepContext;
	class SceneStepContext;
	class GameWorldStepContext;
	class EntityStepContext;

	class Factory
	{
		Factory() = delete;
		Factory(const Factory& toCopy) = delete;
		Factory(Factory&& toMove) = delete;
		~Factory() = delete;
	public:
		class ForProgram
		{
			friend class Program;
			ForProgram() = delete;
			ForProgram(const Factory& toCopy) = delete;
			ForProgram(ForProgram&& toMove) = delete;
			~ForProgram() = delete;
			static Engine* engine(Program& program);
		};
		class ForGameWorld
		{
			friend class ::ssge::GameWorld;
			ForGameWorld() = delete;
			ForGameWorld(const ForGameWorld& toCopy) = delete;
			ForGameWorld(ForGameWorld&& toMove) = delete;
			~ForGameWorld() = delete;
			static ssge::GameWorldStepContext gameWorldStepContext(::ssge::GameWorld& currentGameWorld, ::ssge::SceneStepContext& sceneStepContext);
		};
		class ForEntityManager
		{
			friend class ::ssge::EntityManager;
			ForEntityManager() = delete;
			ForEntityManager(const ForEntityManager& toCopy) = delete;
			ForEntityManager(ForEntityManager&& toMove) = delete;
			~ForEntityManager() = delete;
			static ssge::EntityStepContext entityStepContext(ssge::GameWorldStepContext& gameWorldStepContext);
		};
	};
}