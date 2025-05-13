// Structs.h
// This file contains all major structs

#ifndef StructsIncluded
#define StructsIncluded

// File includes
#include "../Includes/VulkanIncludes.h"
#include "../Includes/GLMIncludes.h"

// Standard library includes
#include <optional>
#include <array>
#include <string>

namespace DDM3
{
	// Struct needed for vulkan setup
	struct QueueFamilyIndices
	{
		// The graphics family that will be used 
		std::optional<uint32_t> graphicsFamily;
		// The present famly that will be used
		std::optional<uint32_t> presentFamily;

		// Function used to see if values have been initialized
		bool isComplete() const
		{
			// Check if both variables have value
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

	// Vertex struct for rendering
	struct Vertex
	{
		// Position
		alignas(16) glm::vec3 pos;
		// Color
		alignas(16) glm::vec3 color;
		// UV coordinates
		alignas(16) glm::vec2 texCoord;
		// Vertex normal
		alignas(16) glm::vec3 normal;
		// Vertex tangent
		alignas(16) glm::vec3 tangent;
		// Bone indices
		alignas(16) glm::vec4 boneIndices;
		// Bone weights
		alignas(16) glm::vec4 boneWeights;
		// Uv set index
		alignas(4) float uvSetIndex{0};

		// Get vulkan binding description
		static VkVertexInputBindingDescription getBindingDescription()
		{
			// Create binding description
			VkVertexInputBindingDescription bindingDescription{};
			// Set binding to 0
			bindingDescription.binding = 0;
			// Set stride to the size of vertex object
			bindingDescription.stride = sizeof(Vertex);
			// Put inputrate as input rate vertex
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			// Return binding description
			return bindingDescription;
		}

		// Get attribute description of vertex
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescription()
		{
			// Create attribute description
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions(8);

			// Set binding to 0
			attributeDescriptions[0].binding = 0;
			// Set location to 0
			attributeDescriptions[0].location = 0;
			// Set format to vector 3
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Set offset
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// Set binding to 0
			attributeDescriptions[1].binding = 0;
			// Set location to 0
			attributeDescriptions[1].location = 1;
			// Set format to vector 3
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Set offset
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			// Set binding to 0
			attributeDescriptions[2].binding = 0;
			// Set location to 2
			attributeDescriptions[2].location = 2;
			// Set format to vector 2
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			// Set offset
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			// Set binding to 0
			attributeDescriptions[3].binding = 0;
			// Set location to 3
			attributeDescriptions[3].location = 3;
			// Set format to vector 3
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Set offset
			attributeDescriptions[3].offset = offsetof(Vertex, normal);

			// Set binding to 0
			attributeDescriptions[4].binding = 0;
			// Set location to 4
			attributeDescriptions[4].location = 4;
			// Set format to vector 3
			attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Set offset
			attributeDescriptions[4].offset = offsetof(Vertex, tangent);

			// Set binding to 0
			attributeDescriptions[5].binding = 0;
			// Set location to 3
			attributeDescriptions[5].location = 5;
			// Set format to vector 3
			attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
			// Set offset
			attributeDescriptions[5].offset = offsetof(Vertex, boneIndices);

			// Set binding to 0
			attributeDescriptions[6].binding = 0;
			// Set location to 4
			attributeDescriptions[6].location = 6;
			// Set format to vector 3
			attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			// Set offset
			attributeDescriptions[6].offset = offsetof(Vertex, boneWeights);

			// Set binding to 0
			attributeDescriptions[7].binding = 0;
			// Set location to 5
			attributeDescriptions[7].location = 7;
			// Set format to float
			attributeDescriptions[7].format = VK_FORMAT_R32_SFLOAT;
			// Set offset
			attributeDescriptions[7].offset = offsetof(Vertex, uvSetIndex);

			// Return attribute descriptions
			return attributeDescriptions;
		}

		// Function for == operator
		// Parameter:
		//     other: Vertex to be compared to
		bool operator==(const Vertex& other) const
		{
			// Compare every attribute seperately
			return pos == other.pos && color == other.color && texCoord == other.texCoord && tangent == other.tangent && normal == other.normal;
		}
	};

	struct AnimationKey
	{
		float tick{};
		std::vector<glm::mat4x4> boneTransforms{};
	};

	struct AnimationClip
	{
		AnimationClip() = default;

		std::wstring name{};
		float duration{};
		float ticksPerSecond{};
		std::vector<AnimationKey> keys{};
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
		alignas(16) glm::vec3 direction{ 1, 0, 0 };
		// Color of light
		alignas(16) glm::vec3 color{ 1, 1, 1 };
		// Intensity of light
		alignas(4) float intensity{ 1 };
	};
#pragma warning(pop)
}


namespace std
{

	// Hash function needed to make an unordered map of vertices

	template<> struct hash<DDM3::Vertex>
	{
		size_t operator()(DDM3::Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1 );
		}
	};
}


#endif // !StructsIncluded