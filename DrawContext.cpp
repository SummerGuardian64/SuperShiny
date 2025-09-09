#include "DrawContext.h"

ssge::DrawContext::DrawContext(Program& program) : program(program)
{
}

ssge::DrawContext::DrawContext(DrawContext&& toMove) : program(toMove.program)
{

}

ssge::DrawContext::~DrawContext()
{
}
