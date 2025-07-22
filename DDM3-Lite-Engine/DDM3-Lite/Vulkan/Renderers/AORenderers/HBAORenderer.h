// HBAORenderer.h
// This renderer will implement Horizon Based Ambient Occlusion

#ifndef _HBAO_RENDERER_
#define _HBAO_RENDERER_

// Parent includde
#include "Vulkan/Renderers/Renderer.h"

namespace DDM3
{
	class HBAORenderer
	{
	public:
		HBAORenderer();

		virtual ~HBAORenderer();

		// Delete copy and move operations
		HBAORenderer(HBAORenderer&) = delete;
		HBAORenderer(HBAORenderer&&) = delete;

		HBAORenderer& operator=(HBAORenderer&) = delete;
		HBAORenderer& operator=(HBAORenderer&& other) = delete;

		virtual void Render();

		virtual VkExtent2D GetExtent();

		virtual RenderpassWrapper* GetDefaultRenderpass();

		virtual void AddDefaultPipelines();
	private:

	};
}

#endif // !_HBAO_RENDERER_
