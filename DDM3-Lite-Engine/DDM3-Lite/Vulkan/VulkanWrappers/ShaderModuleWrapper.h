// ShaderModuleWrapper.h
// This class will load in a shader and use SpirV reflect to get the information needed from the code

#ifndef ShaderModuleWrapperIncluded
#define ShaderModuleWrapperIncluded

// File includes
#include "Includes/VulkanIncludes.h"
#include "Vulkan/SpirVReflect/spirv_reflect.h"

// Standard library includes
#include <string>
#include <vector>
#include <map>


namespace DDM3
{
	class ShaderModuleWrapper
	{
	public:
		// Delete default constructor
		ShaderModuleWrapper() = delete;

		// Constructor
		// Parameters:
		//     device: handle of the logical device
		//     filePath: path to the shader file
		ShaderModuleWrapper(VkDevice device, const std::string& filePath);

		// Default destructor
		~ShaderModuleWrapper() = default;

		// Delete copy and move functions
		ShaderModuleWrapper(ShaderModuleWrapper& other) = delete;
		ShaderModuleWrapper(ShaderModuleWrapper&& other) = delete;
		ShaderModuleWrapper& operator=(ShaderModuleWrapper& other) = delete;
		ShaderModuleWrapper& operator=(ShaderModuleWrapper&& other) = delete;

		// Clean up allocated objects
		// Parameters:
		//     device: handle of the logical device
		void Cleanup(VkDevice device);

		// Return the create info for the shader stage
		VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const { return m_ShaderstageCreateInfo; }

		// Add the descriptor set layout bindings
		// Parameters:
		//     bindings: vector of bindings that this function will add to
		void AddDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& bindings);

		// Add the amount of each descriptor type and the amount of descriptors per binding
		void AddDescriptorInfo(std::map<VkDescriptorType, int>& typeCount, std::map<int, int>& descriptorsPerBinding);

		void AddPushConstantRanges(std::vector<VkPushConstantRange>& pushConstantRanges);

		int GetShaderStage() const { return m_ReflectShaderModule.shader_stage; }
		int GetOutputAmount() const { return m_ReflectShaderModule.output_variable_count; }

	private:
		// The binary code from the shader
		std::vector<char> m_ShaderCode{};

		// The vulkan shader module
		VkShaderModule m_ShaderModule{};

		// The createInfo for the shader stage
		VkPipelineShaderStageCreateInfo m_ShaderstageCreateInfo{};

		// The shade info from spirv-reflect
		SpvReflectShaderModule m_ReflectShaderModule{};

		// Create a vulkan shader module from binary code
		// Parameters:
		//     device: handle of the VkDevice
		void CreateShaderModule(VkDevice device);

		// Read in the shader information with SpirV-reflect
		void ReflectShader();

		// Create the info for the shader stage
		void CreateShaderStageInfo();
	};
}

#endif // !ShaderModuleWrapperIncluded