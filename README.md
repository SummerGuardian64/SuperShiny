# Super Shiny

This game is a devotion to block-smashing platforming action.

Shiny the Scalefloof is my mascot inspired by various cartoons.

Music is mine too, though contains inspirational snippets.

I made the engine too for modern and retro systems.

All C++17 and SDL2 (image+mixer+ttf).

For Windows XP, I used codeblocks-17.12-nosetup IDE with i686-8.1.0-release-win32-dwarf-rt_v6-rev0 toolchain.

Windows 11 works using CMake and Visual Studio 2022.

## Special thanks to my buddies:
- Devee
- Lilithe
- Wilt

**and my mother in the other dimension**

>Hvala ti za sve

## Contact

Visit https://summerguardian64.net for more.

Join my Discord server https://discord.gg/6mWUNC99yP

Thank you for playing!

## Technical info

Currently supported operating systems:

- Windows XP 32-bit
- Windows 11 64-bit

Engine features (as of v0.1.0):

- Decoupled engine components
- Communication between components via Contexts
- Additional encapsulation with PassKeys and Accessors
- InputManager that supports Keyboard and SDL_Joystick (other methods are WIP)
- Borderless Fullscreen, Integral/Fractional scaling setting
- Rebindable controls
- Levels are loaded from INI files
- Settings are saved into config.ini
- Volume control
- Game pauses when joystick is disconnected
- In-house entity management system
- Decoupling game from engine (see main.cpp)
- Exposing engine via Contexts and Accessors so that dev/designer/user code never actually uses the actual engine objects
- Registration of Scenes, Entities and Menus
- Menu system
- Pause system
- Entities have Physics and Physics Abilities to allow for multiple kinds of situations
- Tile-based collision
- Parallax scrolling
- Sprite animation system

The first alpha version is very limited to accommodate for the deadline of Zagreb Games Week 2025.

## Acknowledgements

ChatGPT Plus was used for refactoring, research, debugging, syntax error correction and code review.

WithEXE_Mirror was **entirely** written via my prompt using ChatGPT Plus. I could not waste time on reinventing robocopy.exe for Windows XP just because Microsoft didn't ship it.

Many suddenly copypasted parts of code (e.g. Breakenzi, GML sub-engine) are from my previous projects from when I was daydreaming about one day making my own fully fledged C++ game engine.
