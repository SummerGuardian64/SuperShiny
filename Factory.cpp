#include "Factory.h"
#include "Engine.h"
#include "StepContext.h"

ssge::GameWorldStepContext ssge::Factory::ForGameWorld::gameWorldStepContext(::ssge::GameWorld& currentGameWorld, ::ssge::SceneStepContext& sceneStepContext)
{
    return ssge::GameWorldStepContext(currentGameWorld, sceneStepContext);
}

ssge::Engine* ssge::Factory::ForProgram::engine(Program& program)
{
    return new ssge::Engine(program);
}

ssge::EntityStepContext ssge::Factory::ForEntityManager::entityStepContext(ssge::GameWorldStepContext& gameWorldStepContext)
{
    return ssge::EntityStepContext(gameWorldStepContext);
}
