#pragma once

namespace ssge
{
	enum class SceneClassID
	{
		None,
		SplashScreen,
		TitleScreen,
		GameWorld
	};

	class SceneStepContext;
	class DrawContext;
	
	class Scene
	{
	public:
		Scene() = default;
		virtual SceneClassID getSceneClassID() const = 0;
		virtual void init(ssge::SceneStepContext& context) = 0;
		virtual void step(ssge::SceneStepContext& context) = 0;
		virtual void draw(ssge::DrawContext& context) = 0;
		virtual ~Scene() = default;
	};
};