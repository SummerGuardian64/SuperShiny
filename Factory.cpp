#include "Factory.h"

ssge::StepContext ssge::Factory::ForEngine::stepContext(::ssge::Engine* actualEngine)
{
    return ssge::StepContext(actualEngine);
}

ssge::SceneStepContext ssge::Factory::ForSceneManager::sceneStepContext(::ssge::Scene& currentScene, ::ssge::StepContext& stepContext)
{
    return ssge::SceneStepContext(currentScene, stepContext);
}

ssge::GameWorldStepContext ssge::Factory::ForGameWorld::gameWorldStepContext(::ssge::GameWorld& currentGameWorld, ::ssge::SceneStepContext& sceneStepContext)
{
    return ssge::GameWorldStepContext(currentGameWorld, sceneStepContext);
}

ssge::Engine* ssge::Factory::ForProgram::engine(Program& program)
{
    return new ssge::Engine(program);
}
