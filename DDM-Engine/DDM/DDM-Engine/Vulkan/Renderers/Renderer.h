// Renderer.h
// This class will be the default renderer for everything 

#ifndef _RENDERER_
#define _RENDERER_

// File includes
#include "DDM-Engine/Includes/VulkanIncludes.h"
#include "DDM-Engine/Vulkan/VulkanWrappers/RenderpassWrapper.h"

// Export include
#include "DDM-Engine/Export.h"

// Standard library includes
#include <memory>
#include <vector>

namespace DDM
{
	class DDM_API Renderer
	{
	public:
		Renderer() = default;

		virtual ~Renderer() = default;

		// Delete copy and move operations
		Renderer(Renderer&) = delete;
		Renderer(Renderer&&) = delete;

		Renderer& operator=(Renderer&) = delete;
		Renderer& operator=(Renderer&& other) = delete;

		virtual void Setup() = 0;

		virtual void Render() = 0;

		virtual VkExtent2D GetExtent() = 0;

		virtual RenderpassWrapper* GetDefaultRenderpass() = 0;

		virtual void AddDefaultPipelines() = 0;
	};
}

#endif // !_RENDERER_
