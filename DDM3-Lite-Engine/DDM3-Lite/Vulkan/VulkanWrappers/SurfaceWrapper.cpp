// SurfaceWrapper.cpp

// Header include
#include "SurfaceWrapper.h"

// File includes
#include "Vulkan/VulkanObject.h"
#include "Engine/Window.h"
#include "Vulkan/VulkanWrappers/InstanceWrapper.h"

// Standard library includes
#include <stdexcept>

DDM::SurfaceWrapper::SurfaceWrapper(InstanceWrapper* instance)
{
	m_Instance = instance;
	// Create the surface
	CreateSurface(instance->GetInstance());
}

DDM::SurfaceWrapper::~SurfaceWrapper()
{
	Cleanup(m_Instance->GetInstance());
}

void DDM::SurfaceWrapper::Cleanup(VkInstance instance)
{
	if (m_Surface != VK_NULL_HANDLE)
	{
		// Destroy the surface
		vkDestroySurfaceKHR(instance, m_Surface, nullptr);
		m_Surface = VK_NULL_HANDLE;
	}
}

void DDM::SurfaceWrapper::CreateSurface(VkInstance instance)
{
	// Create the window surface
	if (glfwCreateWindowSurface(instance, Window::GetInstance().GetWindowStruct().pWindow, nullptr, &m_Surface) != VK_SUCCESS)
	{
		// If unsuccessful, throw runtime error
		throw std::runtime_error("failed to create window surface!");
	}
}
