// MenuContext.h
#pragma once
#include "Accessor.h"
#include "PassKey.h"

namespace ssge
{
    class MenuContext {
    public:
        EngineAccess engine;
        ScenesAccess scenes;
        InputsAccess inputs;
        DrawingAccess drawing;
        MenusAccess menus;
        CurrentSceneAccess currentScene;
        GameWorldAccess gameWorld;
        LevelAccess level;

        explicit MenuContext(
            PassKey<GameWorld> pk,
            EngineAccess engine,
            ScenesAccess scenes,
            InputsAccess inputs,
            DrawingAccess drawing,
            MenusAccess menus,
            CurrentSceneAccess currentScene,
            GameWorldAccess gameWorld,
            LevelAccess level
        );
    };
}
