#include "StepContext.h"

using namespace ssge;

// ---- StepContextBase -------------------------------------------------
StepContextBase::StepContextBase(double deltaTime) : deltaTime(deltaTime) {}

// ---- StepContext -----------------------------------------------------
StepContext::StepContext(
    PassKey<Engine> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    GameAccess game_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    MenusAccess menus_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    game(std::move(game_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    menus(std::move(menus_))
{
}

// ---- SceneStepContext -----------------------------------------------
SceneStepContext::SceneStepContext(
    PassKey<SceneManager> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    GameAccess game_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    MenusAccess menus_,
    CurrentSceneAccess currentScene_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    game(std::move(game_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    menus(std::move(menus_)),
    currentScene(std::move(currentScene_))
{
}

// ---- GameWorldStepContext -------------------------------------------
GameWorldStepContext::GameWorldStepContext(
    PassKey<GameWorld> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    GameAccess game_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    CurrentSceneAccess currentScene_,
    GameWorldAccess gameWorld_,
    LevelAccess level_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    game(std::move(game_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    currentScene(std::move(currentScene_)),
    gameWorld(std::move(gameWorld_)),
    level(std::move(level_))
{
}

// ---- EntityStepContext ----------------------------------------------
EntityStepContext::EntityStepContext(
    PassKey<EntityManager> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    GameAccess game_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    CurrentSceneAccess currentScene_,
    GameWorldAccess gameWorld_,
    LevelAccess level_,
    EntitiesAccessWCurrent entitiesAndCurrent_,
    SpritesAccess sprites_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    game(std::move(game_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    currentScene(std::move(currentScene_)),
    gameWorld(std::move(gameWorld_)),
    level(std::move(level_)),
    entities(std::move(entitiesAndCurrent_)),
    sprites(std::move(sprites_))
{
}
