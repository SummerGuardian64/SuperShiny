#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class DrawContext // Encapsulates core objects while calling draw functions
	{
		friend class Program;
	private:
		double deltaTime;
		Program& program;
		DrawContext(Program& program);
		DrawContext(const DrawContext& toCopy) = delete;
		DrawContext(DrawContext&& toMove);
		~DrawContext() = default;
	};

	class SceneManagerDrawContext // Encapsulates core objects while calling draw functions from SceneManager
	{
		friend class Program;
	private:
		double deltaTime;
		Program& program;
		SceneManagerDrawContext(Program& program);
		SceneManagerDrawContext(const SceneManagerDrawContext& toCopy) = delete;
		SceneManagerDrawContext(SceneManagerDrawContext&& toMove);
		~SceneManagerDrawContext() = default;
	};

	class SceneDrawContext // Encapsulates core objects while calling draw functions from Scene
	{
		friend class Program;
	private:
		double deltaTime;
		Program& program;
		SceneDrawContext(Program& program);
		SceneDrawContext(const SceneDrawContext& toCopy) = delete;
		SceneDrawContext(SceneDrawContext&& toMove);
		~SceneDrawContext() = default;
	};
}