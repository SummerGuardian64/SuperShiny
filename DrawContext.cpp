#include "DrawContext.h"

ssge::DrawContext::DrawContext(Program& program) : program(program)
{
	deltaTime = 1.0f / 60.0f;
}

ssge::DrawContext::DrawContext(DrawContext&& toMove) : program(toMove.program)
{

}

// SceneManagerDrawContext

ssge::SceneManagerDrawContext::SceneManagerDrawContext(Program& program) : program(program)
{
	deltaTime = 1.0f / 60.0f;
}

ssge::SceneManagerDrawContext::SceneManagerDrawContext(SceneManagerDrawContext&& toMove) : program(toMove.program)
{

}

// SceneDrawContext

ssge::SceneDrawContext::SceneDrawContext(Program& program) : program(program)
{
	deltaTime = 1.0f / 60.0f;
}

ssge::SceneDrawContext::SceneDrawContext(SceneDrawContext&& toMove) : program(toMove.program)
{

}

