#pragma once

// Super Shiny Game Engine

// Namespace and classes

namespace ssge // Super Shiny Game Engine
{
	class Program;
	class Engine;
	class Game;

	//class Factory;
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

	//class Entity;
	//class Sprite;

	//// Contexts
	class StepContext;
	class DrawContext;
	class SceneManagerStepContext;
	class SceneManagerDrawContext;
	class SceneStepContext;
	class SceneDrawContext;
	//class WorldStepContext;
	//class WorldDrawContext;
	//class EntityStepContext;
	//class EntityDrawContext;
	//...
}

// Header files that belong to Super Shiny Game Engine
#include "Program.h"
//#include "Factory.h"
#include "WindowManager.h"
#include "InputManager.h"
//#include "ResourceManager.h"
#include "StepContext.h"
#include "DrawContext.h"
#include "Scene.h"
#include "SceneManager.h"
//#include "SceneCollection.h"
#include "GameWorld.h"
//#include "GameProgress.h"
//#include "Level.h"
//#include "LevelLoader.h"
//#include "Entity.h"
//#include "Sprite.h"
#include "Engine.h"
#include "Game.h"