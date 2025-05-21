// VulkanRenderer.h

// This class will hold and manage most vulkan objects and  handle rendering

#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

// File includes
#include "Engine/Singleton.h"
#include "DataTypes/Structs.h"

// Standard library includes
#include <iostream>
#include <tuple>
#include <map>
#include <memory>

namespace DDM3
{
    // Class forward declarations
    class ImGuiWrapper;
    class InstanceWrapper;
    class SurfaceWrapper;
    class GPUObject;
    class SyncObjectManager;
    class CommandpoolManager;
    class BufferManager;
    class ImageManager;
    class SwapchainWrapper;
    class RenderpassWrapper;
    class PipelineManager;
    class PipelineWrapper;
    class DescriptorObject;


    class VulkanObject final : public Singleton<VulkanObject>
    {
    public:
        ~VulkanObject();

        VulkanObject(VulkanObject& other) = delete;
        VulkanObject(VulkanObject&& other) = delete;

        VulkanObject& operator=(VulkanObject& other) = delete;
        VulkanObject& operator=(VulkanObject&& other) = delete;

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


        void AddDefaultPipeline();

		GPUObject* GetGPUObject() const { return m_pGpuObject.get(); }

        VkRenderPass GetRenderPass() const;

    private:
        // Constructor
        friend class Singleton<VulkanObject>;
        VulkanObject();

        const size_t m_MaxFramesInFlight{ 2 };

        //----Member variables----

        // ImGui Wrapper
        std::unique_ptr<ImGuiWrapper> m_pImGuiWrapper{};

        // Instance wrapper
        std::unique_ptr<InstanceWrapper> m_pInstanceWrapper{};

        // Surface wrapper
        std::unique_ptr<SurfaceWrapper> m_pSurfaceWrapper{};

        // GPU object
        std::unique_ptr<GPUObject> m_pGpuObject{};

        // Sync object manager
        std::unique_ptr<SyncObjectManager> m_pSyncObjectManager{};

        // CommandpoolManager
        std::unique_ptr<CommandpoolManager> m_pCommandPoolManager{};

        // BufferManager
        std::unique_ptr<BufferManager> m_pBufferManager{};


        // ImageManager
        std::unique_ptr<ImageManager> m_pImageManager{};

        // SwapchainWrapper
        std::unique_ptr<SwapchainWrapper> m_pSwapchainWrapper{};
        
        // RenderpassWrapper
        std::unique_ptr<RenderpassWrapper> m_pRenderpassWrapper{};

        // Pipeline managar
        std::unique_ptr<PipelineManager> m_pPipelineManager{};


        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;


        //--Current frame--
        uint32_t m_CurrentFrame = 0;


        //DefaultTexture
        uint32_t m_MipLevels{};


        //----Member Functions----
        //--- Cleanup ---
        void CleanupVulkan();
        void CleanupImGui();

        //---Initialization---
        void InitVulkan();
        void InitImGui();

        //--Swapchain--
        void RecreateSwapChain();

        //--CommandBuffers--
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);


        //--General helpers--
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer comandBuffer);
    };
}

#endif // !VulkanRendererIncluded