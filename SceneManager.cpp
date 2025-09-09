#include "SceneManager.h"
#include <iostream>

ssge::SceneManager::SceneManager()
{
}

ssge::SceneManager::~SceneManager()
{
}

void ssge::SceneManager::step(StepContext& context)
{
	std::cout << "SceneManager::step()" << std::endl;
}

void ssge::SceneManager::draw(DrawContext& context)
{
	std::cout << "SceneManager::draw()" << std::endl;
}
