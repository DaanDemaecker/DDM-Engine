// VulkanRenderer.h

// This class will hold and manage most vulkan objects and  handle rendering

#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

// File includes
#include "Engine/Singleton.h"
#include "DataTypes/Structs.h"
#include "Vulkan/Renderers/DefaultRenderer.h"

// Standard library includes
#include <iostream>
#include <tuple>
#include <map>
#include <memory>
#include <vector>

namespace DDM3
{
    // Class forward declarations
    class VulkanCore;
    class GPUObject;
    class DefaultRenderer;
    class DescriptorObject;
    class BufferCreator;
    class PipelineManager;


    class VulkanObject final : public Singleton<VulkanObject>
    {
    public:
        ~VulkanObject();

        VulkanObject(VulkanObject& other) = delete;
        VulkanObject(VulkanObject&& other) = delete;

        VulkanObject& operator=(VulkanObject& other) = delete;
        VulkanObject& operator=(VulkanObject&& other) = delete;

        void Init();

        void Terminate();

        void Render();

        // Add a new graphics pipeline
         // Parameters:
         //     pipelineName: the name of the new pipeline
         //     filePaths: a list of shader file names for this pipeline
         //     hasDepthStencil: boolean that indicates if this pipeline needs a depth stencil, true by default
        void AddGraphicsPipeline(const std::string& pipelineName, std::initializer_list<const std::string>&& filePaths,
            bool hasDepthStencil = true);

        //Public getters
        size_t GetMaxFrames() const { return m_MaxFramesInFlight; }

        VkDevice GetDevice();
        
        VkPhysicalDevice GetPhysicalDevice();

        VkImageView& GetDefaultImageView();

        VkSampler& GetSampler();

        DDM3::PipelineWrapper* GetPipeline(const std::string& name = "Default");

        VkCommandBuffer& GetCurrentCommandBuffer();

        uint32_t GetCurrentFrame() const { return  m_CurrentFrame; }
    
        // Create a buffer
        // Parameters:
        //     size: the size of the buffer
        //     usage: the usage flags for the buffer
        //     properties: the property flags for the buffer
        //     buffer: reference to the buffer to be made
        //     bufferMemory: reference to the memory of the buffer that will be made
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        // Copy a buffer to another buffer
        // Parameters:
        //     srcBuffer: the source buffer
        //     dstBuffer: the destination buffer
        //     size: the size of the buffers
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        // Create a vertex buffer
        // Parameters:
        //     vertices: reference to vector of vertices 
        //     vertexBuffer: handle to the vertex buffer to be created
        //     vertexBufferMemory: handle of the vertex buffer memory
        void CreateVertexBuffer(std::vector<DDM3::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

        // Create a vertex buffer
        // Parameters:
        //     indices: reference to vector of indices 
        //     indexBuffer: handle to the index buffer to be created
        //     indexBufferMemory: handle of the index buffer memory
        void CreateIndexBuffer(std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);
     
        // Create a texture
        // Parameters:
        //     texture: reference to the texture object that will hold the texture
        //     textureName: textpath to the image
        void CreateTexture(Texture& texture, const std::string& textureName);

        // Create a cube texture
        // Parameters:
        //     cubeTexture: reference to the texture object
        //     textureNames: a list of the file paths for the cube faces in order: right,left,up,down,front,back
        void CreateCubeTexture(Texture& cubeTexture, const std::initializer_list<std::string const>& textureNames);

        void UpdateUniformBuffer(UniformBufferObject& buffer);

        // Get a pointer to the DescriptorObject of the global light
        DescriptorObject* GetLightDescriptor();

        GPUObject* GetGPUObject() const;

        VkRenderPass GetRenderPass() const;

        int GetCurrentFrame();

        VkSurfaceKHR GetSurface();


       const QueueObject& GetQueueObject();

       VkInstance GetVulkanInstance();

       VkSampleCountFlagBits GetMsaaSamples() { return m_MsaaSamples; }

    private:
        // Constructor
        friend class Singleton<VulkanObject>;
        VulkanObject();

        // Max amount of frames in flight
        static size_t m_MaxFramesInFlight;

        //--Current frame--
        static uint32_t m_CurrentFrame;


        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

        //----Member variables----
        // Vulkan Core
        std::unique_ptr<VulkanCore> m_pVulkanCore{};

        // Renderer
        std::unique_ptr<DefaultRenderer> m_pDefaultRenderer{};

        // Buffer creator
        std::unique_ptr<BufferCreator> m_pBufferCreator{};

        // Pipeline manager
        std::unique_ptr<PipelineManager> m_pPipelineManager{};

        uint32_t m_MipLevels{};

    };
}

#endif // !VulkanRendererIncluded