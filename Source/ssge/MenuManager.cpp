#include "MenuManager.h"

using namespace ssge;

MenuManager::MenuManager(PassKey<Engine> pk, IMenuCommandSink& sink) : sink_(sink) {}

int MenuManager::registerMenu(Menu menu) {
    menus_.push_back(std::move(menu));
    return (int)menus_.size() - 1;
}
void MenuManager::pushMenu(int id) { stack_.push(id); }

void MenuManager::popMenu() { if (!stack_.empty()) stack_.pop(); }

void MenuManager::close()
{
    while (!stack_.empty())
        stack_.pop();
}

bool MenuManager::isOpen() const { return !stack_.empty(); }

const Menu* MenuManager::current() const
{
    if (stack_.empty()) return nullptr;
    return &menus_[stack_.top()];
}

void MenuManager::handleInput(bool up, bool down, bool left, bool right, bool accept, bool back)
{
    if (stack_.empty())
        return;

    Menu& m = menus_[stack_.top()];

    auto advance = [&](int dir)
    {
        if (m.items.empty()) return;
        int i = m.selected;
        for (int step = 0; step < (int)m.items.size(); ++step)
        {
            i = (i + dir + (int)m.items.size()) % (int)m.items.size();
            if (m.items[i].selectable && m.items[i].enabled)
            {
                m.selected = i;
                return;
            }
        }
    };

    if (up)
        advance(-1);

    if (down)
        advance(+1);

    if (left || right)
    {
        MenuItem& it = m.items[m.selected];
        if (it.rightLeftAdjust && it.enabled)
        {
            // emit “adjust” command: left = -1, right = +1
            sink_.onMenuCommand({ MenuCommandType::Option, it.command.param, left ? -1 : +1 });
        }
    }

    if (accept)
    {
        MenuItem& it = m.items[m.selected];
        if (!it.enabled || !it.selectable) return;
        sink_.onMenuCommand(it.command);
    }

    if (back)
    {
        sink_.onMenuCommand(MenuCommand::Pop());
    }
}
void MenuManager::update()
{
    // optional: repeat delays, animations, blink selected, sfx triggers, etc.
}
