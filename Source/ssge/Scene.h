#pragma once
#include <string>

namespace ssge
{
	class SceneStepContext;
	class DrawContext;
	
	class Scene
	{
	public:
		Scene() = default;
		virtual std::string getSceneClassID() const = 0;
		virtual void init(SceneStepContext& context) = 0;
		virtual void step(SceneStepContext& context) = 0;
		virtual void draw(DrawContext& context) = 0;
		virtual ~Scene() = default;
	};
};