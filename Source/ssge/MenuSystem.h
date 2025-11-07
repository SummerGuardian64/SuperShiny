#pragma once
#include <string>
#include <vector>
#include <stack>
#include "SDL.h"
#include "PassKey.h"

namespace ssge
{
    class MenuContext;
    class DrawContext;
    class Engine;

    class MenuManager;
    class MenuHeader;
    class MenuItem;
    class MenuCommand;
    typedef void (*MenuFunction)(int direction);
    class MenuSetting;

    class MenuCommand
    {
    public:
        static const int NOTHING = -1;
        static const int NEW_GAME = -2;
        static const int GOTO_LEVEL = -3;
        static const int NEXT_LEVEL = -4;
        static const int RESTART_LEVEL = -5;
        static const int MAIN_MENU = -6;
        static const int CLOSE_MENU = -7;
        static const int EXIT_PROGRAM = -8;
        static const int SET_MENU = -9;
        static const int MAKE_ROOT = -10;
        static const int SUB_MENU = -11;
        static const int POP_AND_GOTO = -12;
        static const int GO_BACK = -13;
    private:
        int _cmd = NOTHING;
    public:
        MenuCommand& operator=(const int targetLevel);
        MenuCommand& operator=(MenuCommand& other);
        bool operator==(const int other);
        bool operator==(MenuCommand& other);
        MenuCommand(int command = MenuCommand::NOTHING);
        MenuCommand(const MenuCommand& toCopy);
        MenuCommand(MenuCommand&& toMove) noexcept;
        operator int() const;
    };

    class MenuSetting
    {
    public:
        virtual std::string printSetting() const = 0;
        virtual void change(int direction) = 0;
    };
    class MenuSettingInt : public MenuSetting
    {
    public:
        int* integer = nullptr;
        int min = 0;
        int max = 0;
        std::string printSetting() const;
        void change(int direction);
        MenuSettingInt(int* integer = nullptr, int min = 0, int max = 0);
    };
    // TODO: More kinds of MenuSetting's
    // For example, MenuSettingInputBinding
    // Idea. A function is allowed to deactivate input for all
    // other components of the engine while we're waiting for
    // the input that will be mapped.
    class MenuSettingBool : public MenuSetting
    {
    public:
        bool* boolean = nullptr;
        std::string strOn;
        std::string strOff;
        std::string printSetting() const;
        void change(int direction);
        MenuSettingBool(bool* boolean = nullptr, std::string strOn = "On", std::string strOff = "Off");
    };

    class MenuItem
    {
    private:
        std::string text;
    public:
        std::string getText() const;
        bool selectable;
        bool enabled;
        bool visible;
        bool rightLeft;
        MenuCommand command;
        MenuFunction onSelect;
        MenuHeader* targetMenu;
        MenuSetting* setting;
        int execute(int direction);
        bool isSelectable() const;
        bool isEnabled() const;
        bool isRightLeftable() const;
        std::string printItem() const;
        MenuItem();
        MenuItem(
            const char* text = "",
            bool selectable = true,
            bool enabled = true,
            bool visible = true,
            MenuCommand command = MenuCommand::NOTHING,
            MenuHeader* target_menu = NULL,
            bool rightleft = false,
            MenuSetting* setting = NULL,
            MenuFunction on_select = NULL
        );
        ~MenuItem();
    };

    class MenuCommandEx
    {
    public:
        MenuCommand smallCmd = MenuCommand::NOTHING;
        MenuHeader* targetMenu = nullptr;
    };

    class MenuHeader
    {
    private:
        std::string title;
    public:
        std::string getTitle() const;
        void setTitle(const char* text);
        std::vector<MenuItem*> items;
        MenuItem* newLabel(const char* text);
        MenuItem* newItem(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_NewGame(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_RestartLevel(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_SubMenu(const char* text, MenuHeader* targetMenu, MenuFunction onSelect = NULL);
        MenuItem* newItem_GoBack(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_NextLevel(const char* text);
        MenuItem* newItem_MainMenu(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_IntSetting(const char* text, int* setting, int min, int max, MenuCommand command = MenuCommand::NOTHING, MenuFunction = nullptr);
        MenuItem* newItem_BoolSetting(const char* text, bool* setting, MenuCommand command = MenuCommand::NOTHING, MenuFunction = nullptr);
        MenuItem* newItem_BoolSetting(const char* text, bool* setting, std::string strOn, std::string strOff, MenuCommand command = MenuCommand::NOTHING, MenuFunction = nullptr);
        MenuItem* newItem_CloseMenu(const char* text, MenuFunction onSelect = NULL);
        MenuItem* newItem_ExitProgram(const char* text, MenuFunction onSelect = NULL);
        MenuHeader();
        MenuHeader(const char* title);
        ~MenuHeader();
    };

	class MenuManager
	{
    private:
        bool _justOpened = true;
    public:
        // HELP ME REPLACE ALLEGRO_COLOR WITH SOMETHING SDL2 LIKES!
        constexpr static SDL_Color cItemCursor = SDL_Color{ 255, 192, 64, 255 };
        constexpr static SDL_Color cItemSelected = SDL_Color{ 255, 255, 128, 255 };
        constexpr static SDL_Color cItemEnabled = SDL_Color{ 255, 192, 64, 255 };
        constexpr static SDL_Color cItemDisabled = SDL_Color{ 128, 128, 128, 255 };
        constexpr static SDL_Color cItemLabel = SDL_Color{ 255, 255, 255, 255 };
        constexpr static SDL_Color cMenuTitle = SDL_Color{ 255, 64, 32, 255 };
        constexpr static SDL_Color cMenuBackground = SDL_Color{ 32, 32, 192, 64 };
        constexpr static SDL_Color cTextShadow = SDL_Color{ 64, 64, 64, 255 };
        static void drawText(DrawContext& drawContext, SDL_Color color, int height, std::string text);
        int levelSelectorInt = 0;
        MenuHeader* currentMenu = nullptr;
        std::stack<MenuHeader*> previousMenus;
        unsigned int itemIndex = 0;
        void properlyAdjustItemIndex();
        void setMenu(MenuHeader* menu);
        bool isOpen() const;
        void close();
        MenuManager(PassKey<Engine> pk);
        void step(MenuContext& context);
        void draw(DrawContext& context);
	};
}