#include "StepContext.h"

// ---- StepContextBase -------------------------------------------------
ssge::StepContextBase::StepContextBase(double deltaTime) : deltaTime(deltaTime) {}

// ---- StepContext -----------------------------------------------------
ssge::StepContext::StepContext(
    ssge::PassKey<ssge::Engine> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_))
{
}

// ---- SceneStepContext -----------------------------------------------
ssge::SceneStepContext::SceneStepContext(
    ssge::PassKey<ssge::SceneManager> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    CurrentSceneAccess currentScene_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    currentScene(std::move(currentScene_))
{
}

// ---- GameWorldStepContext -------------------------------------------
ssge::GameWorldStepContext::GameWorldStepContext(
    ssge::PassKey<ssge::GameWorld> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    CurrentSceneAccess currentScene_,
    GameWorldAccess gameWorld_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    currentScene(std::move(currentScene_)),
    gameWorld(std::move(gameWorld_))
{
}

// ---- EntityStepContext ----------------------------------------------
ssge::EntityStepContext::EntityStepContext(
    ssge::PassKey<ssge::EntityManager> /*pk*/,
    double deltaTime,
    EngineAccess engine_,
    ScenesAccess scenes_,
    InputsAccess inputs_,
    DrawingAccess drawing_,
    CurrentSceneAccess currentScene_,
    GameWorldAccess gameWorld_,
    EntitiesAccess entities_
)
    : StepContextBase(deltaTime),
    engine(std::move(engine_)),
    scenes(std::move(scenes_)),
    inputs(std::move(inputs_)),
    drawing(std::move(drawing_)),
    currentScene(std::move(currentScene_)),
    gameWorld(std::move(gameWorld_)),
    entities(std::move(entities_))
{
}
