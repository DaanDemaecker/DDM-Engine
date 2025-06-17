// TextureDescriptorObject.h
// This class wil handle the updating of texture descriptor sets
#ifndef TextureDescriptorObjectIncluded
#define TextureDescriptorObjectIncluded


// File includes
#include "DescriptorObject.h"
#include "../../DataTypes/Structs.h"

// Standard includes
#include <initializer_list>
#include <string>


namespace DDM3
{
    class TextureDescriptorObject final : public DescriptorObject
    {
    public:
        // Default constructor
        TextureDescriptorObject();

        void AddTextures(const Texture& texture);

        void AddTextures(std::initializer_list<const std::string>& filePaths);

        void AddTexture(const std::string& filePath);
         
        void Clear();

        size_t GetTextureAmount() const;

        // Destructor
        virtual ~TextureDescriptorObject();

        void SetCleanupTextures(bool cleanupTextures) { m_CleanupTextures = cleanupTextures; }

        // Add the descriptor write objects to the list of descriptorWrites
        // Parameters:
        //     descriptorSet: the current descriptorset connected to this descriptor object
        //     descriptorWrites: the list of descriptorWrites this function will add to
        //     binding: the current binding in the shader files
		//     amount: the amount of descriptors for the current binding
        //     index: the current frame index of the renderer
        virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index) override;
    private:
        bool m_CleanupTextures{ true };

        // List of the textures
        std::vector<Texture> m_Textures{};
        // List of image info per texture
        std::vector<VkDescriptorImageInfo> m_ImageInfos{};

        VkDescriptorImageInfo m_PlaceholderImageInfo{ VkDescriptorImageInfo()};

        // Set up an image info object for each texture
        void SetupImageInfos();
    };
}

#endif // !TextureDescriptorObjectIncluded