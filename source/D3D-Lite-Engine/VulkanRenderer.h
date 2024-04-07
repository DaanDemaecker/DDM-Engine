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
        VkCommandPool& GetCommandPool() { return m_CommandPool; }
        VkImageView& GetDefaultImageView() { return m_DefaultTextureImageView; }
        VkSampler& GetSampler() { return m_TextureSampler; }
        D3D::PipelinePair& GetPipeline(const std::string& name = "Default");
        VkCommandBuffer& GetCurrentCommandBuffer() { return m_CommandBuffers[m_CurrentFrame]; }
        uint32_t GetCurrentFrame() const { return  m_CurrentFrame; }
        DescriptorPoolManager* GetDescriptorPoolManager() const;
        const LightObject& GetGlobalLight() const { return m_GlobalLight; }
        std::vector<VkBuffer>& GetLightBuffers() { return m_LightBuffers; }

        //Public Helpers
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
        void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
            VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

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
        std::unique_ptr<GPUObject> m_pGPUObject{};

        //--Swapchain--
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        //-Swapchain MinImageCount-
        uint32_t m_MinImageCount{};

        //-Swapchain Images-
        std::vector<VkImage> m_SwapChainImages{};

        //-Swapchain Image Format-
        VkFormat m_SwapChainImageFormat{};

        //-Swapchain Extent-
        VkExtent2D m_SwapChainExtent{};

        //--Image Views--
        std::vector<VkImageView> m_SwapChainImageViews{};

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

        //--CommandPool--
        VkCommandPool m_CommandPool{};

        //CommandBuffer
        std::vector<VkCommandBuffer> m_CommandBuffers{};


        //--MultiSampling--
        VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkImage m_ColorImage{};
        VkDeviceMemory m_ColorImageMemory{};
        VkImageView m_ColorImageView{};

        //--Depth Image--
        VkImage m_DepthImage{};
        VkDeviceMemory m_DepthImageMemory{};
        VkImageView m_DepthImageView{};

        //--Framebuffers--
        std::vector<VkFramebuffer> m_SwapChainFramebuffers{};

        //--Descriptorpool--
        std::unique_ptr<DescriptorPoolManager> m_pDescriptorPoolManager{};

        //--Sync objects--
        std::vector<VkSemaphore> m_ImageAvailableSemaphores{};
        std::vector<VkSemaphore> m_RenderFinishedSemaphores{};
        std::vector<VkFence> m_InFlightFences{};


        //--Current frame--
        uint32_t m_CurrentFrame = 0;


        //DefaultTexture
        VkImage m_DefaultTextureImage{};
        VkDeviceMemory m_DefaultTextureImageMemory{};
        VkImageView m_DefaultTextureImageView{};
        uint32_t m_MipLevels{};

        const std::string m_DefaultTextureName{ "../resources/DefaultResources/DefaultTexture.png" };

        VkSampler m_TextureSampler{};



        //----Member Functions----
        //--- Cleanup ---
        void CleanupVulkan();
        void CleanupImGui();

        //---Initialization---
        void InitVulkan();
        void InitImGui();

        //--Swapchain--
        void CreateSwapChain();

        //-Swapchain Helper Functions-
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void CleanupSwapChain();
        void RecreateSwapChain();

        //--Swapchain Image Views--
        void CreateImageViews();

        //--RenderPass--
        void CreateRenderPass();

        //--Descriptor Layout
        void CreateDescriptorLayout(int vertexUbos, int fragmentUbos, int textureAmount);

        //-Shader Module-
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        //--Command Pool
        void CreateCommandPool();

        //--MultiSampling--
        void CreateColorResources();

        //--Depth Image--
        void CreateDepthResources();

        //--Framebuffers--
        void CreateFramebuffers();

        //--CommandBuffers--
        void CreateCommandBuffers();
        void RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

        //--Sync Objects--
        void CreateSyncObjects();

        //--LightBuffers--
        void CreateLightBuffer();
        void UpdateLightBuffer(int frame);


        //--General helpers--
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer comandBuffer);
        bool HasStencilComponent(VkFormat format);


        //Texture functions
        void CreateTextureImage();
        void CreateTextureImageView();
        void CreateTextureSampler();
    };
}

#endif // !VulkanRendererIncluded