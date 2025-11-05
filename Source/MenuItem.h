#pragma once
#include <string>
#include "MenuCommand.h"

namespace ssge
{
	struct MenuItem
	{
		std::string label;      // visible text
		bool selectable{ true };
		bool enabled{ true };
		bool rightLeftAdjust{ false }; // left/right changes a value
		MenuCommand command;    // the action
		// Optional: callback to mutate engine config, but prefer command dispatch for determinism
	};
} // namespace ssge
