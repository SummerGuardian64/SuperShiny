// MenuContext.h
#pragma once
#include "Accessor.h"
#include "PassKey.h"

namespace ssge
{
    class Engine;

    class MenuContext {
    public:
        EngineAccess engine;
        GameAccess game;
        WindowAccess window;
        ScenesAccess scenes;
        InputsAccessConfigurable inputs;
        DrawingAccess drawing;
        MenusAccess menus;
        CurrentSceneAccess currentScene;
        GameWorldAccess gameWorld;
        LevelAccess level;

        explicit MenuContext(
            PassKey<Engine> pk,
            EngineAccess engine,
            GameAccess game,
            WindowAccess window,
            ScenesAccess scenes,
            InputsAccessConfigurable inputs,
            DrawingAccess drawing,
            MenusAccess menus,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level
        );
    };
}
