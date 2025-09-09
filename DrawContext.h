#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class DrawContext // Encapsulates core objects while calling draw functions
	{
		friend class Program;
	private:
		Program& program;
		DrawContext(Program& program);
		DrawContext(const DrawContext& toCopy) = delete;
		DrawContext(DrawContext&& toMove);
		~DrawContext();
	};
}