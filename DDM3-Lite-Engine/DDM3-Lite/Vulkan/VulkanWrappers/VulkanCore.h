// VulkanCore.h
// This class will hold the core components of vulkan like the VkInstance, VkDevice etc.

#ifndef _VULKAN_CORE_
#define _VULKAN_CORE_

// File includes
#include "Includes/VulkanIncludes.h"

// Standare library includes
#include <memory>

namespace DDM3
{
	// Class forward declarations
	class InstanceWrapper;
	class GPUObject;
	class SurfaceWrapper;

	struct QueueObject;

	class VulkanCore final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		VulkanCore();

		~VulkanCore() = default;

		/// <summary>
		/// Get the Vulkan instance
		/// </summary>
		/// <returns>VkInstance handle</returns>
		VkInstance GetVulkanInstance();

		/// <summary>
		/// Get the Vulkan surface
		/// </summary>
		/// <returns>Vulkan surface</returns>
		VkSurfaceKHR GetSurface();

		/// <summary>
		/// Get a pointer to the GPU object
		/// </summary>
		/// <returns>Pointer to GPUObject class</returns>
		GPUObject* GetGpuObject();

		/// <summary>
		/// Get Vulkan physical device
		/// </summary>
		/// <returns>VkPhyiscalDevice handle</returns>
		VkPhysicalDevice GetPhysicalDevice();

		/// <summary>
		/// Get Vulkan logical device
		/// </summary>
		/// <returns>VkDevice handle</returns>
		VkDevice GetDevice();
		
		/// <summary>
		/// Get reference to struct that holds present and graphics queue
		/// </summary>
		/// <returns>Reference to QueueObject</returns>
		const QueueObject& GetQueueObject();

	private:
		// Wrapper for VkInstance
		std::unique_ptr<InstanceWrapper> m_pInstance;

		// Wrapper for VkSurface
		std::unique_ptr<SurfaceWrapper> m_pSurface;

		// Wrapper for VkDevice and VkPhysicalDevice
		std::unique_ptr<GPUObject> m_pGpuObject;
	};
}

#endif // !_VULKAN_CORE_
