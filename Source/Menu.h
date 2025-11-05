#pragma once
#include <vector>
#include <string>
#include "MenuItem.h"

namespace ssge
{
	struct Menu
	{
		std::string title;
		std::vector<MenuItem> items;
		int selected{ 0 };
		// optional: layout hints
		int itemSpacing = 36;
	};
} // namespace ssge
