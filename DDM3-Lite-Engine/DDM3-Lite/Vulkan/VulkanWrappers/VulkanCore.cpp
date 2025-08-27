// VulkanCore.cpp

// Header include
#include "VulkanCore.h"

// File includes
#include "Vulkan/VulkanWrappers/InstanceWrapper.h"
#include "Vulkan/VulkanWrappers/GPUObject.h"
#include "Vulkan/VulkanWrappers/SurfaceWrapper.h"

#include "DataTypes/Structs.h"


DDM::VulkanCore::VulkanCore()
{
	// Intialize instance wrapper
	m_pInstance = std::make_unique<DDM::InstanceWrapper>();

	InstanceWrapper* pInstance = m_pInstance.get();

	// Initialize surface
	m_pSurface = std::make_unique<DDM::SurfaceWrapper>(pInstance);

	// Initialize gpu object
	m_pGpuObject = std::make_unique<DDM::GPUObject>(m_pInstance.get(), m_pSurface->GetSurface());
	
}

VkInstance DDM::VulkanCore::GetVulkanInstance()
{
	return m_pInstance->GetInstance();
}

VkSurfaceKHR DDM::VulkanCore::GetSurface()
{
	return m_pSurface->GetSurface();
}

DDM::GPUObject* DDM::VulkanCore::GetGpuObject()
{
	return m_pGpuObject.get();
}

VkPhysicalDevice DDM::VulkanCore::GetPhysicalDevice()
{
	return m_pGpuObject->GetPhysicalDevice();
}

VkDevice DDM::VulkanCore::GetDevice()
{
	return m_pGpuObject->GetDevice();
}

const DDM::QueueObject& DDM::VulkanCore::GetQueueObject()
{
	return m_pGpuObject->GetQueueObject();
}
