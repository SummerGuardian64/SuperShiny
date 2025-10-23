#pragma once
#include <cstdint>

namespace ssge {

	enum class MenuCommandType {
		None,
		NewGame,
		GoToLevel,
		PushMenu,
		Option,
		Pop,
		Close,
		ExitProgram,
		TOTAL
	};

	struct MenuCommand {
		MenuCommandType type = MenuCommandType::None;
		int32_t param = 0; // e.g. level index or menu id
		int direction = 0; // for rightleftable
		static MenuCommand NewGame() { return { MenuCommandType::NewGame }; }
		static MenuCommand GoToLevel(int lvl) { return { MenuCommandType::GoToLevel, lvl }; }
		static MenuCommand PushMenu(int id) { return { MenuCommandType::PushMenu, id }; }
		static MenuCommand Option(int option, int direction) { return { MenuCommandType::Option, option, direction }; }
		static MenuCommand Pop() { return { MenuCommandType::Pop }; }
		static MenuCommand Close() { return { MenuCommandType::Close }; }
		static MenuCommand ExitProgram() { return { MenuCommandType::ExitProgram }; }
	};

	struct IMenuCommandSink {
		virtual ~IMenuCommandSink() = default;
		virtual void onMenuCommand(const MenuCommand& cmd) = 0;
	};

} // namespace ssge
