#include "MenuContext.h"
#include "Engine.h"

using namespace ssge;

MenuContext::MenuContext(
	PassKey<Engine> pk,
	EngineAccess engine,
	GameAccess game,
	WindowAccess window,
	ScenesAccess scenes,
	InputsAccessConfigurable inputs,
	DrawingAccess drawing,
	MenusAccess menus,
	CurrentSceneAccess currentScene,
	GameWorldAccess gameWorld,
	LevelAccess level
) :
	engine(engine),
	game(game),
	window(window),
	scenes(scenes),
	inputs(inputs),
	drawing(drawing),
	menus(menus),
	currentScene(currentScene),
	gameWorld(gameWorld),
	level(level)
{}
