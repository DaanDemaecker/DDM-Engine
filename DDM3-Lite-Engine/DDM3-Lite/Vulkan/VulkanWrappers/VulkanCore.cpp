// VulkanCore.cpp

// Header include
#include "VulkanCore.h"

// File includes
#include "Vulkan/VulkanWrappers/InstanceWrapper.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"
#include "Vulkan/VulkanWrappers/SurfaceWrapper.h"

#include "DataTypes/Structs.h"


DDM3::VulkanCore::VulkanCore()
{
	// Intialize instance wrapper
	m_pInstance = std::make_unique<DDM3::InstanceWrapper>();

	InstanceWrapper* pInstance = m_pInstance.get();

	// Initialize surface
	m_pSurface = std::make_unique<DDM3::SurfaceWrapper>(pInstance);

	// Initialize gpu object
	m_pGpuObject = std::make_unique<DDM3::GPUObject>(m_pInstance.get(), m_pSurface->GetSurface());
	
}

VkInstance DDM3::VulkanCore::GetVulkanInstance()
{
	return m_pInstance->GetInstance();
}

VkSurfaceKHR DDM3::VulkanCore::GetSurface()
{
	return m_pSurface->GetSurface();
}

DDM3::GPUObject* DDM3::VulkanCore::GetGpuObject()
{
	return m_pGpuObject.get();
}

VkPhysicalDevice DDM3::VulkanCore::GetPhysicalDevice()
{
	return m_pGpuObject->GetPhysicalDevice();
}

VkDevice DDM3::VulkanCore::GetDevice()
{
	return m_pGpuObject->GetDevice();
}

const DDM3::QueueObject& DDM3::VulkanCore::GetQueueObject()
{
	return m_pGpuObject->GetQueueObject();
}
