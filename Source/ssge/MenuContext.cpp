#include "MenuContext.h"
#include "Engine.h"

using namespace ssge;

MenuContext::MenuContext(
	PassKey<GameWorld> pk,
	EngineAccess engine,
	ScenesAccess scenes,
	InputsAccess inputs,
	DrawingAccess drawing,
	MenusAccess menus,
	CurrentSceneAccess currentScene,
	GameWorldAccess gameWorld,
	LevelAccess level
) :
	engine(engine),
	scenes(scenes),
	inputs(inputs),
	drawing(drawing),
	menus(menus),
	currentScene(currentScene),
	gameWorld(gameWorld),
	level(level)
{}
