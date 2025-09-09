#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class StepContext // Encapsulates core objects while calling step functions
	{
		friend class Program;
	private:
		StepContext(ssge::Program& program);
		StepContext(const StepContext& toCopy) = delete;
		StepContext(StepContext&& toMove) = default;
		~StepContext();
	public:
		class Program // Encapsulates the Program
		{
			friend class StepContext;
		private:
			ssge::Program* actualProgram;
			Program(ssge::Program* actualProgram);
			Program(const Program& toCopy) = delete;
			Program(Program&& toMove) = default;
			~Program();
		public:
			// Request exit from the main loop
			void exit();
		};
		class InputManager
		{
			friend class StepContext;
		private:
			ssge::InputManager* actualInputManager;
			InputManager(ssge::InputManager* actualInputManager);
			InputManager(const InputManager& toCopy) = delete;
			InputManager(InputManager&& toMove) = default;
			~InputManager();
		};
		Program program;
	};
}