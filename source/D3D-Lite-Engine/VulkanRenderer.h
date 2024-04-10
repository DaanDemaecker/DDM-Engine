#ifndef VulkanRendererIncluded
#define VulkanRendererIncluded

#include "Singleton.h"
#include "Structs.h"

#include <iostream>
#include <tuple>
#include <map>
#include <memory>

namespace D3D
{
    // Class forward declarations
    class ModelComponent;
    class ImGuiWrapper;
    class DescriptorPoolManager;
    class InstanceWrapper;
    class SurfaceWrapper;
    class GPUObject;
    class SyncObjectManager;
    class CommandpoolManager;
    class BufferManager;
    class ImageManager;
    class SwapchainWrapper;

    class VulkanRenderer final : public Singleton<VulkanRenderer>
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        VulkanRenderer(VulkanRenderer& other) = delete;
        VulkanRenderer(VulkanRenderer&& other) = delete;

        VulkanRenderer& operator=(VulkanRenderer& other) = delete;
        VulkanRenderer& operator=(VulkanRenderer&& other) = delete;

        void Render();

        void Render(ModelComponent* pModel, VkCommandBuffer& commandBuffer, const VkDescriptorSet* descriptorSet, const PipelinePair& pipeline);

        void AddGraphicsPipeline(const std::string& pipelineName, const std::string& vertShaderName, const std::string& fragShaderName, int vertexUbos, int fragmentUbos, int textureAmount);
       
        //Public getters
        size_t GetMaxFrames() const { return m_MaxFramesInFlight; }
        VkDevice GetDevice();
        VkImageView& GetDefaultImageView();
        VkSampler& GetSampler();
        D3D::PipelinePair& GetPipeline(const std::string& name = "Default");
        VkCommandBuffer& GetCurrentCommandBuffer();
        uint32_t GetCurrentFrame() const { return  m_CurrentFrame; }
        DescriptorPoolManager* GetDescriptorPoolManager() const;
        const LightObject& GetGlobalLight() const { return m_GlobalLight; }
        std::vector<VkBuffer>& GetLightBuffers() { return m_LightBuffers; }

    
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
        void CreateVertexBuffer(std::vector<D3D::Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

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

        VkDescriptorSetLayout* GetDescriptorSetLayout(int vertexUbos, int fragmentUbos, int textureAmount);

    private:
        const size_t m_MaxFramesInFlight{ 2 };

        LightObject m_GlobalLight{};
        std::vector<bool> m_LightChanged{};

        std::vector<VkBuffer> m_LightBuffers{};
        std::vector<VkDeviceMemory> m_LightMemory{};
        std::vector<void*> m_LightMapped{};
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
        

        //--Renderpass--
        VkRenderPass m_RenderPass{};

        //-RenderpassInfo-
        VkRenderPassBeginInfo m_RenderpassInfo{};

        //--Descriptorlayouts--
        std::map<std::tuple<int, int, int>, VkDescriptorSetLayout> m_DescriptorSetLayouts{};

        //--GraphicsPipeline--
        //VkPipeline m_GraphicsPipeline{};
        std::map<std::string, PipelinePair> m_GraphicPipelines{};

        const std::string m_DefaultPipelineName{ "Default" };
        const std::string m_DefaultVertName{ "../resources/DefaultResources/Default.Vert.spv" };
        const std::string m_DefaultFragName{ "../resources/DefaultResources/Default.Frag.spv" };


        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

        //--Descriptorpool--
        std::unique_ptr<DescriptorPoolManager> m_pDescriptorPoolManager{};


        //--Current frame--
        uint32_t m_CurrentFrame = 0;


        //DefaultTexture
        uint32_t m_MipLevels{};

        const std::string m_DefaultTextureName{ "../resources/DefaultResources/DefaultTexture.png" };


        //----Member Functions----
        //--- Cleanup ---
        void CleanupVulkan();
        void CleanupImGui();

        //---Initialization---
        void InitVulkan();
        void InitImGui();

        //--Swapchain--
        void RecreateSwapChain();

        //--RenderPass--
        void CreateRenderPass();

        //--Descriptor Layout
        void CreateDescriptorLayout(int vertexUbos, int fragmentUbos, int textureAmount);

        //-Shader Module-
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        //--CommandBuffers--
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

        //--LightBuffers--
        void CreateLightBuffer();
        void UpdateLightBuffer(int frame);


        //--General helpers--
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer comandBuffer);
        bool HasStencilComponent(VkFormat format);
    };
}

#endif // !VulkanRendererIncluded