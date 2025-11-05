#pragma once
#include <vector>
#include <stack>
#include "Menu.h"
#include "MenuCommand.h"
#include "PassKey.h"

namespace ssge
{
	class Engine;

	class MenuManager
	{
	public:
		explicit MenuManager(PassKey<Engine> pk, IMenuCommandSink& sink);

		// Data-driven registration (IDs to Menus)
		int registerMenu(Menu menu);         // returns id
		void pushMenu(int menuId);
		void popMenu();
		void close();
		bool isOpen() const;
		const Menu* current() const;

		// Controller: input is abstracted as intent booleans
		void handleInput(bool up, bool down, bool left, bool right, bool accept, bool back);
		void update(); // if you want animations, blink, repeat delays, etc.

	private:
		std::vector<Menu> menus_;
		std::stack<int> stack_;
		IMenuCommandSink& sink_;
	};
} // namespace ssge
