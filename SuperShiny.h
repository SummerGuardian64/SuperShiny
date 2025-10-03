#pragma once

// Super Shiny Game Engine

// Namespace and classes

namespace ssge // Super Shiny Game Engine
{
	class Program;
	class Engine;
	class Game;

	class Factory;
	class WindowManager;
	class InputManager;
	//class ResourceManager;

	class SceneManager;
	//class SceneCollection;
	class Scene;
	enum class SceneClassID : int;
	//
	class GameWorld;
	//class GameProgress;

	//class Level;
	//class LevelLoader;

	class EntityCollection;
	class EntityReference;
	class EntityAllocator;
	class EntityManager;
	enum class EntityClassID : int;
	class Entity;
	//class Sprite;

	// StepContexts
	class StepContext;
	class SceneStepContext;
	class GameWorldStepContext;
	class EntityStepContext;
	//...

	// DrawContexts
	class DrawContext;
}

#include "Program.h"
