#pragma once
#include "SuperShiny.h"

using namespace ssge;

namespace ssge
{
	enum class SceneClassID
	{
		None,
		SplashScreen,
		TitleScreen,
		GameWorld
	};

	class Scene
	{
	public:
		Scene() = default;
		virtual SceneClassID getSceneClassID() const = 0;
		virtual void init(SceneStepContext& context) = 0;
		virtual void step(SceneStepContext& context) = 0;
		virtual void draw(SceneDrawContext& context) = 0;
		virtual ~Scene() = default;
	};
};