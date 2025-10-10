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
	};
}