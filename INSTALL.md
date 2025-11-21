# Installation guide

## For players (quick version)

If you want to play the latest version of Super Shiny,
you need to download one of the available ZIP files below,
then extract the ZIP file entirely to a local folder where you want to keep the game.

- [Download Super Shiny v0.1.2 for Windows XP 32-bit](https://github.com/SummerGuardian64/SuperShiny/releases/download/v0.1.2/SuperShiny_WindowsXP_x86_Release.zip)
- [Download Super Shiny v0.1.2 for Windows 11 64-bit](https://github.com/SummerGuardian64/SuperShiny/releases/download/v0.1.2/SuperShiny_Windows11_x64_Release.zip)

The game is 100% portable and doesn't require any administrator rights to run.

That means you can put it on a USB stick and play on any machine without installing.

**DO NOT** try running only the EXE file from the ZIP file! All files that are inside of the ZIP file need to be extracted to run the game!

## For developers, modders, code reviewers and whoever wants to compile the code

Super Shiny game and engine currently have only two targets:

- Windows XP 32-bit
- Windows 11 64-bit

The project uses these dependencies:

- SDL2-2.32.8
- SDL2_image-2.8.8
- SDL2_ttf-2.24.0
- SDL2_mixer-2.8.1

**DO NOT** use SDL3 as it is a completely new version of SDL which may not work.

Please use at least the specified version of SDL2 for best results.

---

### Setup for Windows XP

Windows XP builds are made using CodeBlocks (`codeblocks-17.12-nosetup`) as the IDE
and MinGW 8.1.0 (`i686-8.1.0-release-win32-dwarf-rt_v6-rev0`) as the toolchain.

This is the last version of CodeBlocks that works on Windows XP,
but it doesn't come with the MinGW version we need.
Therefore, we download CodeBlocks and MinGW separately and then configure CodeBlocks to detect MinGW.

It is recommended to keep the installation portable to avoid integration hell.

If you don't have a proper modern anti-malware solution for your Windows XP machine,
please use another machine to download all the necessary files to a USB stick
that you can use to transfer the files safely to the Windows XP machine.

Additionally, you may need to download software for extracting ZIP and 7Z files.

---

#### Downloads

- **SuperShiny game & engine source code**: [SuperShiny](https://github.com/SummerGuardian64/SuperShiny/archive/refs/tags/v0.1.2.zip)
- **CodeBlocks 17.12**: [codeblocks-17.12-nosetup](https://sourceforge.net/projects/codeblocks/files/Binaries/17.12/Windows/codeblocks-17.12-nosetup.zip/download)
- **MinGW 8.1.0**: [i686-8.1.0-release-win32-dwarf-rt_v6-rev0](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/threads-win32/dwarf/i686-8.1.0-release-win32-dwarf-rt_v6-rev0.7z/download)
- **SDL2**: [SDL2-devel-2.32.10-mingw](https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-devel-2.32.10-mingw.zip)
- **SDL2_image**: [SDL2_image-devel-2.8.8-mingw](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-mingw.zip)
- **SDL2_ttf**: [SDL2_ttf-devel-2.24.0-mingw](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-mingw.zip)
- **SDL2_mixer**: [SDL2_mixer-devel-2.8.1-mingw](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-mingw.zip)

---

#### Folder structure

Extract these files to a folder whose path you can ensure will stay intact.

If you need to move the folder, you will need to reconfigure the paths.

Recommended folder structure:

+ `D:/`
  + `Dev/`
	+ `codeblocks-17.12-nosetup`
	+ `i686-8.1.0-release-win32-dwarf-rt_v6-rev0`
	+ `SDL2-devel-2.32.10-mingw`
	+ `SDL2_image-devel-2.8.8-mingw`
	+ `SDL2_ttf-devel-2.24.0-mingw`
	+ `SDL2_mixer-devel-2.8.1-mingw`
	+ `Projects/`
	  + `SuperShiny`

It is strongly recommended that you avoid spaces in the path as some utilities may fail.

---

#### Configuring CodeBlocks for the first time

Having extracted all archives according to the scheme, it is time to run CodeBlocks.exe from within its folder and start setting it up.

Upon running CodeBlocks, you will be asked to select a compiler. We cannot select the compiler from here because it is not automatically detected. Simply click OK to skip the automatic setup.

CodeBlocks window will open fully now and you will be asked if you want to associate development files with CodeBlocks. This is up to you.

Now let's set up the compiler:

1. Go to *Settings* > *Compiler...*
2. In *Global compiler settings* section, in *Selected compiler* dropdown menu, select *GNU GCC Compiler*
3. Then click *Copy*
4. Type in `MinGW 8.1.0 for XP` (this is very important!) and click *OK*
5. With **MinGW 8.1.0 for XP** now selected, click *Toolchain executables* tab
6. In *Compiler's installation directory* groupbox, click the *...* button
7. Navigate to your compiler's `mingw32` folder (e.g. `D:\Dev\i686-8.1.0-release-win32-dwarf-rt_v6_rev0\mingw32`)
8. Click *Program Files* tab
9. Type in the following names:

| Label                    | Entry                      |
| ------------------------ | -------------------------- |
| C compiler:              | gcc.exe                    |
| C++ compiler:            | g++.exe                    |
| Linker for dynamic libs: | g++.exe                    |
| Linker for static libs:  | ar.exe                     |
| Debugger:                | GDB/CDB debugger : Default |
| Resource compiler:       | windres.exe                |
| Make program:            | mingw32-make.exe           |
10. And click *OK*

---

Now let's set up the Global Variables for CodeBlocks so it can see SDL2 libraries.

1. Go to *Settings* > *Global Variables...*
2. Under *Current variable* group box, click *New*
3. Type `sdl2` for variable name and press `OK`
4. Under *Built-in fields:*, click the *...* button
5. Navigate to `SDL2-devel.2.32.10.mingw\SDL2-2.32.10\i686-w64-mingw32`
6. Repeat the same process to match the following results:

| Current variable      | Built-in fields:                                               |
| --------------------- | -------------------------------------------------------------- |
| sdl2                  | SDL2-devel.2.32.10.mingw\SDL2-2.32.10\i686-w64-mingw32         |
| sdl2_image            | SDL2_image-devel-2.8.8-mingw\SDL_image-2.8.8\i686-w64-mingw32  |
| sdl2_mixer            | SDL2_mixer-devel-2.8.1-mingw\SDL2_mixer-2.8.1\i686-w64-mingw32 |
| sdl2_ttf              | SDL2_ttf-devel-2.24.0-mingw\SDL2_ttf-2.24.0\i686-w64-mingw32   |
7. Click *Close*

---

#### Testing CodeBlocks

Now that all that is set, it's time to test if CodeBlocks can compile SDL2.

1. Go to *File* > *New* > *Project...*
2. Select *SDL2 Project* and click *OK*
3. Follow the wizard by pressing *Next*
4. Name a test project (e.g. `SDL2Test`)
5. Set project folder to be `Projects\` according to the *Folder structure* using the *...* button (e.g. `D:\Dev\Projects\`) and click *Next*
6. Specify SDL2's location (e.g. `D:\Dev\SDL2-devel-2.32.10-mingw\SDL2-2.32.10\i686-w64-mingw32`) using the *...* button
7. Select **MinGW 8.1.0 for XP** as the compiler and click *Finish*

You will be presented with a test program. Let's compile and run.

1. Go to *Build* > *Build and run*

You should be seeing a window pop up with a graphical test. That means that SDL2 is working.

You may see an error saying
> `fatal error: SDL.h: No such file or directory`

If so, change the erroneous line to `#include <SDL2/SDL.h>` and try again.

If problems persist, please look up the *Issues* tab of this repository for more information.

---

#### Opening SuperShiny CodeBlocks project

Now that CodeBlocks and SDL2 work together, it is time to open SuperShiny.

1. Go to *File* > *Open...*
2. Navigate to the `SuperShiny\` folder you've extracted the source code into and open `SuperShiny.cbp`
3. In the *Management* pane, in the *Projects* tab, make sure that *SuperShiny* is bold. If it's not, right click it and click *Activate project*
4. Go to *Build* > *Build and run*
5. Wait for the project to build

If all goes well, a window will open with a splash screen of Shiny.

This means that you've built the project. **Congratulations!**

Press Enter to proceed to the game.

If you can see the background and the menu, and if you can hear the background music, that means that the dependencies have also been configured.

---

#### Troubleshooting the build process

If you see missing DLL errors, that means that there is an issue with how paths are configured or you haven't extracted the archives properly.

Make sure your paths match the *Folder structure* specified in this document.

If you see build errors, make sure you are using the compiler we've set up (**MinGW 8.1.0 for XP**).

1. In the *Management* pane, in the *Projects* tab, right-click *SuperShiny*
2. Click *Build options...*
3. On the left section of the new window, click `SuperShiny`
4. In the *Selected compiler* dropdown, select **MinGW 8.1.0 for XP**
5. Click *OK*
6. Go to *Build* > *Build and run*

---

If any issue persists, please look up the *Issues* tab of this repository for more information.

---

### Setup for Windows 11

Windows 11 builds are made using Visual Studio 2022 and MSVC v143 - VS 2022 C++ x64/x86 build tools and CMake.

#### Downloads

You need to download and install Visual Studio 2022 and select the following components within Visual Studio Installer:
- MSVC v143 - VS 2022 C++ x64/x86 build tools
- C++ Build Insights
- Just-In-Time Debugger
- C++ profiling tools
- C++ CMake tools for Windows
- IntelliCode
- C++ AddressSanitizer
- Windows 11 SDK
- Git for Windows

After that, you need to download the following:

- **SuperShiny game & engine source code**: [SuperShiny](https://github.com/SummerGuardian64/SuperShiny/archive/refs/tags/v0.1.2.zip)
- **SDL2**: [SDL2-devel-2.32.10-VC](https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-devel-2.32.10-VC.zip)
- **SDL2_image**: [SDL2_image-devel-2.8.8-VC](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-VC.zip)
- **SDL2_ttf**: [SDL2_ttf-devel-2.24.0-VC](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-VC.zip)
- **SDL2_mixer**: [SDL2_mixer-devel-2.8.1-VC](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-VC.zip)

Extract these files to a folder whose path you can ensure will stay intact.

If you need to move the folder, you will need to reconfigure the paths.

If you wish to contribute, you may want to clone the repository:
1. Navigate [to the repository's *Code* tab](https://github.com/SummerGuardian64/SuperShiny)
2. Click the green *Code* button
3. Click *Open with Visual Studio*

#### Setting up the dependencies

By default, neither Visual Studio nor this project see the SDL2 dependencies.

You need to set up the environment variables first, then restart Visual Studio so it may see them.

1. In Windows, press the Start button
2. Start typing `Edit environment variables for your account` and choose the said option
3. Inside of the *Environment Variables* window, add a variable with the name `CMAKE_PREFIX_PATH`
4. For the value, type paths to SDL2, SDL2_image, SDL2_ttf and SDL2_mixer, separating them with a semicolon
5. Press *OK*

For example:
> `D:\Shared\Development\SDL2-2.32.8;D:\Shared\Development\SDL2_image-2.8.8;D:\Shared\Development\SDL2_ttf-2.24.0;D:\Shared\Development\SDL2_mixer-2.8.1`

#### Having set up the dependencies

Now relaunch Visual Studio.

1. In the *Standard* toolbar, near the green arrow button, make sure it says `Super Shiny`. If it doesn't, press the small arrow pointing down on it and select `Super Shiny`
2. Press the green arrow button
3. Wait for the project to build


If all goes well, a window will open with a splash screen of Shiny.

This means that you've built the project. **Congratulations!**

Press Enter to proceed to the game.

If you can see the background and the menu, and if you can hear the background music, that means that the dependencies have also been configured.

#### Troubleshooting the build process

...

---

If any issue persists, please look up the *Issues* tab of this repository for more information.

---

### Super Shiny is built. Now what?

The folder `Export/` contains the final folder(s) with the game that you've built.

You may now copy or move this folder to where you keep your portable games.

**WARNING!**

When building the project, the game settings will be reset. This is because they're stored inside the `config.ini` of the game's root folder.

You are advised to back up this file during development. This may be improved in the future.

**WARNING2!!**

**DO NOT MAKE EDITS WITHIN THE `Export/` FOLDER!!!**

These will be overwritten any time you build the project.

You should make all edits within the `WithEXE/` folder. As the name says, all contents there will be copied where the final EXE file is.

---

#### CONGRATULATIONS!

**You've just built Super Shiny game & engine!**

Please enjoy the scalefloof~

---