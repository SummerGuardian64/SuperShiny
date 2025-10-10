#include "Factory.h"
#include "Engine.h"
#include "StepContext.h"

ssge::Engine* ssge::Factory::ForProgram::engine(Program& program)
{
    return new ssge::Engine(program);
}

ssge::EntityStepContext ssge::Factory::ForEntityManager::entityStepContext(ssge::GameWorldStepContext& gameWorldStepContext)
{
    return ssge::EntityStepContext(gameWorldStepContext);
}
