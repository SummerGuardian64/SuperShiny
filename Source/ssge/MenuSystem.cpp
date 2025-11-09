#include "MenuSystem.h"
#include "MenuContext.h"
#include "DrawContext.h"
#include <algorithm>
#include "SDL.h"
#include "SDL_ttf.h"

using namespace ssge;

MenuCommand& MenuCommand::operator=(const MenuCommand& other)
{
	_cmd = other._cmd;
	return *this;
}

MenuCommand& MenuCommand::operator=(const int targetLevel)
{
	_cmd = targetLevel;
	return *this;
}

bool MenuCommand::operator==(const int other)
{
	return _cmd == other;
}

bool MenuCommand::operator==(MenuCommand& other)
{
	return _cmd == other._cmd;
}

MenuCommand::MenuCommand(int command) : _cmd(command) {}

MenuCommand::MenuCommand(const MenuCommand& toCopy)
{
	_cmd = toCopy._cmd;
}

MenuCommand::MenuCommand(MenuCommand&& toMove) noexcept
{
	_cmd = toMove._cmd;
	toMove._cmd = NOTHING;
}

MenuCommand::operator int() const
{
	return _cmd;
}

MenuSettingInt::MenuSettingInt(int* integer, int min, int max)
	: integer(integer), min(min), max(max)
{}

std::string MenuSettingInt::printSetting() const
{
    // If setting not initialized, say N/A
	if (!integer)return " N/A";
	
    // Otherwise, print space and then the integer as a string
	return std::string(" ") + std::to_string(*integer);
}

std::string MenuSettingBindingIndex::getText() const
{
    return text;
}

void ssge::MenuSettingBindingIndex::setText(std::string text)
{
    this->text = text;
}

int MenuSettingBindingIndex::getBindingIndex() const
{
    return bindingIndex;
}

std::string MenuSettingBindingIndex::printSetting() const
{
    return std::string(" = ") + text;
}

void MenuSettingBindingIndex::change(int direction)
{
    // We don't do the binding here
    return;
}

MenuSettingBindingIndex::MenuSettingBindingIndex(int bindingIndex)
    :bindingIndex(bindingIndex)
{}

void MenuSettingInt::change(int direction)
{
	// Don't run if direction equals zero
	if (!direction) return;
	// Don't run if pointer is NULL
	if (!integer) return;
	// Don't run if minimum is greater than maximum
	if (min > max) return;
	// Set the new value depending on direction and the bounds
	*integer = std::clamp(*integer + ((direction > 0) ? 1 : -1), min, max);
}

MenuSettingBool::MenuSettingBool(bool* boolean, std::string strOn, std::string strOff)
    : boolean(boolean), strOn(strOn), strOff(strOff)
{}

std::string MenuSettingBool::printSetting() const
{
    // If setting not initialized, say N/A
    if (!boolean)return " N/A";

    // Otherwise, print space
    // and then the on string or the off string depending on the setting
    return std::string(" ") + (*boolean ? strOn : strOff);
}

void MenuSettingBool::change(int direction)
{
    // Don't run if pointer is null
    if (!boolean) return;

    bool& theSetting = *boolean;

    // If we press RIGHT while setting is OFF 
    if (direction > 0 && theSetting == false)
    { // We turn it ON
        theSetting = true;
    }

    // If we press LEFT while setting is ON
    if (direction < 0 && theSetting == true)
    { // We turn it OFF
        theSetting = false;
    }

    // If we simply activated the item
    if (direction == 0)
    { // We TOGGLE it
        theSetting = !theSetting;
    }
}

std::string MenuItem::getText() const
{
	return text;
}

int MenuItem::execute(int direction)
{
	// The item has to be selectable, enabled and visible in order to be executed
	if ((selectable && enabled && visible) == false)
	{
		return MenuCommand::NOTHING;
	}

	// rightLeft has to be set in order to use left/right
	if (!rightLeft && direction != 0)
	{
		return MenuCommand::NOTHING;
	}

	// If onSelect function is set, run it
	if (onSelect)
	{
		(*onSelect)(direction);
	}

    // Change a setting if any exists
	if (setting)
	{
		setting->change(direction);
	}

    // Return the item's command
    return command;
}

bool MenuItem::isSelectable() const
{
	return selectable && visible;
}

bool MenuItem::isEnabled() const
{
	return selectable && visible && enabled;
}

bool MenuItem::isRightLeftable() const
{
	return selectable && visible && rightLeft && enabled;
}

std::string MenuItem::printItem() const
{
	std::string itemText = text;
	if (setting) itemText += setting->printSetting();
	return itemText;
}

MenuItem::MenuItem()
{
	text = "";
	selectable = true;
	enabled = true;
	visible = true;
	rightLeft = false;
	command = MenuCommand::NOTHING;
	onSelect = NULL;
	targetMenu = NULL;
	setting = NULL;
}

MenuItem::MenuItem(const char* text, bool selectable, bool enabled, bool visible, MenuCommand command, MenuHeader* target_menu, bool rightleft, MenuSetting* setting, MenuFunction on_select)
	:
	text(text),
	selectable(selectable),
	enabled(enabled),
	visible(visible),
	rightLeft(rightleft),
	command(command),
	onSelect(on_select),
	targetMenu(target_menu),
	setting(setting)
{}

MenuItem::~MenuItem()
{
	if (setting)delete setting;
}

std::string MenuHeader::getTitle() const
{
	return title;
}

void MenuHeader::setTitle(const char* text)
{
	this->title = text;
}

MenuItem* MenuHeader::newLabel(const char* text)
{
	auto item = new MenuItem(text, false);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::NOTHING, NULL, false, NULL, onSelect);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_NewGame(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::NEW_GAME);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_RestartLevel(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::RESTART_LEVEL);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_SubMenu(const char* text, MenuHeader* targetMenu, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::SUB_MENU, targetMenu, false, NULL, onSelect);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_GoBack(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::GO_BACK, NULL, false, NULL, onSelect);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_NextLevel(const char* text)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::NEXT_LEVEL);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_MainMenu(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::MAIN_MENU, NULL, false, NULL, onSelect);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_IntSetting(const char* text, int* setting, int min, int max, MenuCommand command, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, command, NULL, true, new MenuSettingInt(setting, min, max), onSelect);
	items.push_back(item);
	return item;
}

MenuItem* ssge::MenuHeader::newItem_BoolSetting(const char* text, bool* setting, MenuCommand command, MenuFunction onSelect)
{
    auto item = new MenuItem(text, true, true, true, command, NULL, true, new MenuSettingBool(setting), onSelect);
    items.push_back(item);
    return item;
}

MenuItem* ssge::MenuHeader::newItem_BoolSetting(const char* text, bool* setting, std::string strOn, std::string strOff, MenuCommand command, MenuFunction onSelect)
{
    auto item = new MenuItem(text, true, true, true, command, NULL, true, new MenuSettingBool(setting, strOn, strOff), onSelect);
    items.push_back(item);
    return item;
}

MenuItem* ssge::MenuHeader::newItem_InputBinding(const char* bindingName, int bindingIndex)
{
    auto item = new MenuItem(bindingName, true, true, true, MenuCommand::BIND_INPUT, nullptr, false, new MenuSettingBindingIndex(bindingIndex), nullptr);
    items.push_back(item);
    return nullptr;
}

MenuItem* MenuHeader::newItem_CloseMenu(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::CLOSE_MENU);
	items.push_back(item);
	return item;
}

MenuItem* MenuHeader::newItem_ExitProgram(const char* text, MenuFunction onSelect)
{
	auto item = new MenuItem(text, true, true, true, MenuCommand::EXIT_PROGRAM);
	items.push_back(item);
	return item;
}

MenuHeader::MenuHeader() {}

MenuHeader::MenuHeader(const char* title)
	:title(title)
{}

MenuHeader::~MenuHeader() {}

// Draw a UTF-8 string centered at xCenter,y with an optional shadow.
// Returns the drawn text width/height via outW/outH (nullable).
static void DrawTextCenteredShadowed(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    int xCenter,
    int y,
    SDL_Color color,
    bool drawShadow,
    SDL_Color shadowColor,
    int shadowOffsetPx,
    int* outW = nullptr,
    int* outH = nullptr)
{
    if (!renderer || !font || !text) return;

    // Measure first to compute the centered x
    int w = 0, h = 0;
    if (TTF_SizeUTF8(font, text, &w, &h) != 0) {
        // If measure fails, bail gracefully
        return;
    }
    int x = xCenter - (w / 2);

    // Small lambda to render one colored copy
    auto renderOnce = [&](SDL_Color c, int dx, int dy) {
        SDL_Surface* srf = TTF_RenderUTF8_Blended(font, text, c);
        if (!srf) return;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, srf);
        SDL_FreeSurface(srf);
        if (!tex) return;
        SDL_Rect dst{ x + dx, y + dy, w, h };
        SDL_RenderCopy(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
        };

    if (drawShadow) renderOnce(shadowColor, shadowOffsetPx, shadowOffsetPx);
    renderOnce(color, 0, 0);

    if (outW) *outW = w;
    if (outH) *outH = h;
}

void MenuManager::drawText(DrawContext& dc, SDL_Color color, int height, std::string text)
{
    SDL_Renderer* r = dc.getRenderer();
    // choose your menu font (store it somewhere central)
    TTF_Font* font = dc.getFont(); // or however you access it right now

    // Shadow color similar to your Allegro cTextShadow
    SDL_Color shadow{ 64, 64, 64, 255 };

    const SDL_Rect bounds = dc.getBounds(); // screen or viewport
    const int xCenter = bounds.x + bounds.w / 2;

    DrawTextCenteredShadowed(
        r, font, text.c_str(),
        xCenter, height,
        color,
        /*drawShadow=*/true,
        shadow, /*shadowOffsetPx=*/2
    );
}


void MenuManager::properlyAdjustItemIndex()
{
	if (!currentMenu) return;

	itemIndex = 0;
	if (currentMenu->items.size() > 0)
	{
		for (auto& item : currentMenu->items)
		{
			if (item->isSelectable())
			{
				// Found the first selectable item
				return;
			}
			itemIndex++;
		}
	}
	// There is no selectable item
	itemIndex = 0;
}

void MenuManager::setMenu(MenuHeader* menu)
{
    if (!currentMenu)
    {
        _justOpened = true;
    }
	if (menu)
	{
		currentMenu = menu;
		properlyAdjustItemIndex();
	}
}

bool MenuManager::isOpen() const
{
    return currentMenu != nullptr;
}

void MenuManager::close()
{
    currentMenu = nullptr; // Point to no menu
    while (!previousMenus.empty())previousMenus.pop(); // Forget previous menus
    while (!previousIndexes.empty())previousIndexes.pop(); // ...and indexes!
}

MenuManager::MenuManager(PassKey<Engine> pk)
{
	_justOpened = true;
}

void MenuManager::step(MenuContext& context)
{
    // If there's no current menu, bail out!
    if (!currentMenu) return;

    // Avoid the situation where the player presses a button to open the menu
    // and then the same button does something in the menu on the same frame!
    // *cough* Nokia featurephones *cough*
    if (_justOpened) { _justOpened = false; return; }

    // If not just created, then proceed to the menu controls.

    int returnedCommand = MenuCommand::NOTHING; // A menu item will return a MenuCommand
    bool closeMenu = false; // Should the menu close?
    MenuItem* currentMenuItem = nullptr; // A pointer to simplify redundant code

    if (currentMenu->items.size() > 0)
    {
        if (itemIndex >= currentMenu->items.size())
        {
            itemIndex = 0;
        }
        currentMenuItem = currentMenu->items[itemIndex];
    }

    if (context.inputs.isJustPressed(0))
    {
        if (currentMenuItem) // Check if an item has been selected
        {
            int oldIndex = itemIndex; // Remember the previous index to prevent infinite loop
            do { // Seek a selectable item while making sure to not go out of bounds
                if (itemIndex-- == 0)itemIndex = (unsigned int)currentMenu->items.size() - 1;
                currentMenuItem = currentMenu->items[itemIndex]; // Update current menu item pointer!
                if (currentMenuItem->isSelectable())break;
            } while (oldIndex != itemIndex); // Prevent infinite loop
        }
    }
    else if (context.inputs.isJustPressed(1))
    {
        if (currentMenuItem) // Check if an item has been selected
        {
            int oldIndex = itemIndex; // Remember the previous index to prevent infinite loop
            do { // Seek a selectable item while making sure to not go out of bounds
                if (++itemIndex == currentMenu->items.size())itemIndex = 0;
                currentMenuItem = currentMenu->items[itemIndex]; // Update current menu item pointer!
                if (currentMenuItem->isSelectable())break;
            } while (oldIndex != itemIndex); // Prevent infinite loop
        }
    }
    else if (context.inputs.isJustPressed(2))
    {
        if (currentMenuItem) // Check if an item has been selected
        {
            if (currentMenuItem->isRightLeftable())
            { // If the item supports left/right controls, execute its function.
                returnedCommand = currentMenuItem->execute(-1);
            }
        }
    }
    else if (context.inputs.isJustPressed(3))
    {
        if (currentMenuItem)
        {
            if (currentMenuItem->isRightLeftable())
            { // If the item supports left/right controls, execute its function.
                returnedCommand = currentMenuItem->execute(1);
            }
        }
    }
    else if (context.inputs.isJustPressed(4))
    {
        if (currentMenuItem)
        {
            if (currentMenuItem->isEnabled())
            { // If the item is enabled, execute its function.
                returnedCommand = currentMenuItem->execute(0);
            }
        }
    }
    else if (context.inputs.isJustPressed(5))
    { // Escape key is for going back.
        returnedCommand = MenuCommand::GO_BACK;
    }

    // Update input binding items' labels
    for (auto& item : currentMenu->items)
    {
        if (item->command == MenuCommand::BIND_INPUT)
        { // This one ought to have a setting
            if (auto setting = item->setting)
            { // Let's hope it's a binding index type
                if (auto bindingIndexSetting
                    = dynamic_cast<MenuSettingBindingIndex*>(setting))
                { // Get its binding index
                    int bindingIndex =
                        bindingIndexSetting->getBindingIndex();
                    // Update its text
                    bindingIndexSetting->setText(
                        context.inputs.getBindingString(bindingIndex)
                    );
                }
            }
        }
    }

    // Make up an elaborate ("extended") command
    MenuCommandEx cmdEx;
    cmdEx.smallCmd = returnedCommand;
    cmdEx.targetMenu = currentMenuItem ? currentMenuItem->targetMenu : nullptr;

    // We may need to repeat handling if IGame issues another command!
    bool repeatHandling;

    do
    {
        repeatHandling = false;

        // Handle the returned command.
        switch (cmdEx.smallCmd)
        {
        case MenuCommand::NOTHING:
            // 'Nuff said...
            break;

        case MenuCommand::NEW_GAME:
            // Start a new game
            // TODO: context.progress.newGame()
            context.scenes.goToLevel(1); // and wipe this line
            closeMenu = true;
            break;

        case MenuCommand::GOTO_LEVEL:
            // Go to a level specified by the level selector integer in the menu system
            context.scenes.goToLevel(levelSelectorInt);
            closeMenu = true; // Close menu
            break;

        case MenuCommand::NEXT_LEVEL:
            // Advance to the next level
            //context.scenes.nextLevel(); // TODO: UNIMPLEMENTED!
            closeMenu = true; // Close menu
            break;

        case MenuCommand::RESTART_LEVEL:
            // Reset progress and restart the level
            // TODO: UNIMPLEMENTED!
            //context.progress.reset(); // May not be the right thing!
            context.scenes.restart(); // May need refinement
            closeMenu = true;
            break;

        case MenuCommand::MAIN_MENU:
            // Exit the game, reset the progress and go to the main menu
            //context.progress.reset();
            context.scenes.goToMainMenu();
            closeMenu = true;
            break;

        case MenuCommand::CLOSE_MENU:
            // Close this menu
            closeMenu = true;
            break;

        case MenuCommand::EXIT_PROGRAM:
            // Fade out towards shutting down the program
            context.engine.wrapUp();
            closeMenu = true;
            break;

        case MenuCommand::SET_MENU:
            // Go to the specified target menu
            if (!currentMenuItem)break; // Validate item
            if (!cmdEx.targetMenu)break; // Validate target pointer
            setMenu(cmdEx.targetMenu); // Set target menu for current menu
            break;

        case MenuCommand::MAKE_ROOT:
            // Make the speficied target menu the root menu and go to it
            if (!currentMenuItem)break; // Validate item
            if (!cmdEx.targetMenu)break; // Validate target pointer
            setMenu(cmdEx.targetMenu); // Set target menu for current menu
            // Forget previous menus and indexes
            while (!previousMenus.empty())previousMenus.pop();
            while (!previousIndexes.empty())previousIndexes.pop();
            break;

        case MenuCommand::SUB_MENU:
            // Go to the specified target menu while remembering the way back
            if (!currentMenuItem)break; // Validate item
            if (!cmdEx.targetMenu)break; // Validate target pointer
            previousMenus.push(currentMenu); // Remember current menu
            previousIndexes.push(itemIndex); // ...and cursor position!
            setMenu(cmdEx.targetMenu); // Set target menu for current menu
            break;

        case MenuCommand::POP_AND_GOTO:
            // Go to the specified target menu treating it as the previous menu
            if (!currentMenuItem)break; // Validate item
            if (!cmdEx.targetMenu)break; // Validate target pointer
            // Forget the previous menu
            if (!previousMenus.empty())previousMenus.pop();
            if (!previousIndexes.empty())previousIndexes.pop();
            setMenu(cmdEx.targetMenu); // Set target menu for current menu
            break;

        case MenuCommand::GO_BACK:
            // Go to the previous menu
            if (previousMenus.empty())
            { // If there was no previous menu
                // Ask the game what to do
                cmdEx = context.game.onHavingBackedOutOfMenus(PassKey<MenuManager>(), context);
                // Repeat the handling process
                repeatHandling = true;
            }
            else
            { // If there is a previous menu
                setMenu(previousMenus.top()); // Set the previous menu for current menu
                previousMenus.pop(); // That menu is no longer previous
                itemIndex = previousIndexes.top(); // Restore index
                previousIndexes.pop(); // ...and it's not previous anymore
                // Sanitize item index
                unsigned int nOItems = (unsigned int)currentMenu->items.size();
                if (nOItems == 0)
                { // If there are no items, set index to 0
                    itemIndex = 0;
                }
                else if (itemIndex >= nOItems)
                { // If number of items somehow shrunk
                    // Select last item
                    itemIndex = nOItems - 1;
                }
            }
            break;
        case MenuCommand::BIND_INPUT:
            // Bind the input command
            if (!currentMenuItem)break; // Validate item
            if (currentMenuItem->command == MenuCommand::BIND_INPUT)
            { // This one ought to have a setting
                if (auto setting = currentMenuItem->setting)
                { // Let's hope it's a binding index type
                    if (auto bindingIndexSetting
                        = dynamic_cast<MenuSettingBindingIndex*>(setting))
                    { // Get its binding index
                        int bindingIndex =
                            bindingIndexSetting->getBindingIndex();
                        // Tell InputManager to listen for this binding
                        context.inputs.listenForBinding(bindingIndex);
                    }
                }
            }
            break;
        default:
            // This is for any command number not covered previously
            if (returnedCommand >= 0)
            { // Any non-negative number means to go to that level
                context.scenes.goToLevel(returnedCommand);
            }
            closeMenu = true; // Close this menu
            break;
        }

        if (closeMenu)
        { // If the menu should close,
            close();
        }
    }while (repeatHandling);
}

void MenuManager::draw(DrawContext& context)
{
    if (!currentMenu) return;

    SDL_Renderer* renderer = context.getRenderer();
    const SDL_Rect bounds = context.getBounds();

    // Allegro: al_get_font_line_height(Breakenzi::Font)
    // SDL_ttf equivalent:
    TTF_Font* font = context.getFont(); // or however you access your menu font
    const int fontHeight = font ? TTF_FontLineSkip(font) : 18; // safe fallback

    // Count visible items
    int visibleItems = 0;
    for (auto& item : currentMenu->items)
        if (item->visible) visibleItems++;

    const int menuCenter = bounds.h / 2;
    const int linePadding = 6;
    const int lineHeight = fontHeight + linePadding;
    const int lineCount = visibleItems + 1; // title + items
    const int menuTextHeight = lineCount * lineHeight;
    const int top = menuCenter - menuTextHeight / 2;

    // Background (Allegro had a rounded rect; we fill a rect here)
    // If you want rounded corners later, you can draw 4 quarter-circles + 3 rects,
    // or use SDL2_gfx. Keeping it simple for now.
    const int margin = 10;
    SDL_Rect bg{ bounds.x,
                 top - margin,
                 bounds.w,
                 menuTextHeight + 2 * margin };

    // Save/restore draw color around fills
    Uint8 pr, pg, pb, pa;
    SDL_GetRenderDrawColor(renderer, &pr, &pg, &pb, &pa);
    SDL_SetRenderDrawColor(renderer, cMenuBackground.r, cMenuBackground.g, cMenuBackground.b, cMenuBackground.a);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, pr, pg, pb, pa);

    // Title
    drawText(context, cMenuTitle, top + linePadding / 2, currentMenu->getTitle());

    // Items
    int currentHeight = top + lineHeight;
    std::string stringToPrint;
    SDL_Color itemColor;
    unsigned int i = 0;

    for (auto& item : currentMenu->items)
    {
        // Skip invisible items
        if (!item->visible) { ++i; continue; }

        // Selected row highlight (Allegro: al_draw_filled_rectangle)
        if (i == itemIndex)
        {
            SDL_GetRenderDrawColor(renderer, &pr, &pg, &pb, &pa);
            SDL_SetRenderDrawColor(renderer, cItemCursor.r, cItemCursor.g, cItemCursor.b, cItemCursor.a);
            SDL_Rect sel{ bounds.x, currentHeight, bounds.w, lineHeight };
            SDL_RenderFillRect(renderer, &sel);
            SDL_SetRenderDrawColor(renderer, pr, pg, pb, pa);
        }

        // Determine item color (matches your logic)
        if (item->selectable)
        {
            if (item->enabled)
                itemColor = (i == itemIndex) ? cItemSelected : cItemEnabled;
            else
                itemColor = cItemDisabled;
        }
        else
        {
            itemColor = cItemLabel; // label
        }

        // Text
        stringToPrint = item->printItem();
        drawText(context, itemColor, currentHeight + linePadding / 2, stringToPrint);

        // advance
        currentHeight += lineHeight;
        ++i;
    }
}

