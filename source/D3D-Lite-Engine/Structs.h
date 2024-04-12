#ifndef StructsIncluded
#define StructsIncluded


#include "VulkanIncludes.h"
#include "GLMIncludes.h"

#include <optional>
#include <array>

namespace D3D
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct QueueObject
	{
		//-Graphics queue-
		VkQueue graphicsQueue{};

		//-Graphics queue index-
		uint32_t graphicsQueueIndex{};

		//-Present queue-
		VkQueue presentQueue{};
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats{};
		std::vector<VkPresentModeKHR> presentModes{};
	};

	struct PipelinePair
	{
		VkPipeline pipeline{};
		VkPipelineLayout pipelineLayout{};
	};

	// Struct for compacting vulkan textures
	struct Texture
	{
		// VkImage object
		VkImage image{};
		// VkDeviceMemory object
		VkDeviceMemory imageMemory{};
		// VkImageView object
		VkImageView imageView{};
		// The amount of levels the mipmap will have
		uint32_t mipLevels{};

		// Cleanup function
		// Parameters: 
		//     device: handle to VkDevice
		virtual void Cleanup(VkDevice device)
		{
			// Destroy the image view
			vkDestroyImageView(device, imageView, nullptr);
			// Destroy the image
			vkDestroyImage(device, image, nullptr);
			// Free the memory
			vkFreeMemory(device, imageMemory, nullptr);
		}
	};

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
		glm::vec3 normal;
		glm::vec3 tangent;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescription()
		{
			std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, normal);

			attributeDescriptions[4].binding = 0;
			attributeDescriptions[4].location = 4;
			attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[4].offset = offsetof(Vertex, tangent);


			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	// Uniform buffer object
		// Needed for transformations in shaders
	struct UniformBufferObject
	{
		// Transformation of model
		glm::mat4 model{};
		// Transformation of camera
		glm::mat4 view{};
		// Transformation needed to put modle in projection space
		glm::mat4 proj{};
	};

#pragma warning(push)
	// Disable warning C4324
#pragma warning(disable : 4324)
	// Directional light object
	struct DirectionalLightStruct
	{
		// Direction of light
		alignas(16) glm::vec3 direction;
		// Color of light
		alignas(16) glm::vec3 color;
		// Intensity of light
		alignas(4) float intensity;
	};
#pragma warning(pop)
}


namespace std
{
	template<> struct hash<D3D::Vertex>
	{
		size_t operator()(D3D::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1 );
		}
	};
}


#endif // !StructsIncluded