// SurfaceWrapper.h
// This class will hold and handle all VkSurfaceKHR stuff

#ifndef SurfaceWrapperIncluded
#define SurfaceWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"

namespace DDM
{
	// Class forward declarations
	class InstanceWrapper;

	class SurfaceWrapper final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="instance: ">Pointer to an instance wrapper object</param>
		SurfaceWrapper(InstanceWrapper* instance);

		// Delete default constructor
		SurfaceWrapper() = delete;

		// Default destructor
		~SurfaceWrapper();

		// Delete copy and move functions
		SurfaceWrapper(SurfaceWrapper& other) = delete;
		SurfaceWrapper(SurfaceWrapper&& other) = delete;
		SurfaceWrapper& operator=(SurfaceWrapper& other) = delete;
		SurfaceWrapper& operator=(SurfaceWrapper&& other) = delete;

		// Get the handle of the surface
		VkSurfaceKHR GetSurface() const { return m_Surface; }
	private:
		// Handle of the VkSurfaceKHR
		VkSurfaceKHR m_Surface{};

		InstanceWrapper* m_Instance;

		// Initialize the VkSurfaceKHR
		// Parameters:
		//     instance: The handle of the Vulkan instance
		void CreateSurface(VkInstance instance);

		// Clean up allocated objects
		// Parameters:
		//     instance: The handle of the Vulkan instance
		void Cleanup(VkInstance instance);
	};
}

#endif // !SurfaceWrapperIncluded