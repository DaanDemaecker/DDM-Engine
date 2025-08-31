// TextureDescriptorObject.h
// This class wil handle the updating of texture descriptor sets

#ifndef _TEXTURE_DESCRIPTOR_OBJECT_
#define _TEXTURE_DESCRIPTOR_OBJECT_


// File includes
#include "DescriptorObject.h"
#include "DataTypes/Structs.h"

// Standard includes
#include <initializer_list>
#include <string>


namespace DDM
{
    class TextureDescriptorObject final : public DescriptorObject
    {
    public:
        /// <summary>
        /// Default constructor
        /// </summary>
        TextureDescriptorObject();

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~TextureDescriptorObject();
        
        /// <summary>
        /// Add a texture to the list
        /// </summary>
        /// <param name="texture: ">reference to the texture to add</param>
        void AddTexture(const Texture& texture);

        /// <summary>
        /// Add a list of textures
        /// </summary>
        /// <param name="filePaths: ">list of filepaths to images</param>
        void AddTextures(std::initializer_list<const std::string>& filePaths);

        /// <summary>
        /// Add a single texture
        /// </summary>
        /// <param name="filePath: ">file path to the image</param>
        void AddTexture(const std::string& filePath);
         
        /// <summary>
        /// Clear the list of textures
        /// </summary>
        void Clear();

        /// <summary>
        /// Get the amount textures currently in the list
        /// </summary>
        /// <returns>Amount of textures</returns>
        size_t GetTextureAmount() const;

        /// <summary>
        /// Indicate wether the textures should be cleaned up at the end
        /// </summary>
        /// <param name="cleanupTextures: ">indicates wether to clean up textures</param>
        void SetCleanupTextures(bool cleanupTextures) { m_CleanupTextures = cleanupTextures; }

        /// <summary>
        /// Add the descriptor write objets to the list of descriptorWrites
        /// </summary>
        /// <param name="descriptorSet: ">current descriptorset connected to this descriptor object</param>
        /// <param name="descriptorWrites: ">list of descriptorWrites to add to</param>
        /// <param name="binding: ">current binding in the shader files</param>
        /// <param name="amount: ">amount of elements in the array at this binding</param>
        /// <param name="index">index of the current frame in flight</param>
        virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index) override;
    private:
        // Indicates wether the image infos are set up
        bool m_AreImageInfosSetup{ true };

        // Indicates wether textures should be cleaned up at the end
        bool m_CleanupTextures{ true };

        // List of the textures
        std::vector<Texture> m_Textures{};

        // List of image info per texture
        std::vector<VkDescriptorImageInfo> m_ImageInfos{};

        // Placeholder image info
        VkDescriptorImageInfo m_PlaceholderImageInfo{ VkDescriptorImageInfo()};

        /// <summary>
        /// Set up the image infos with given textures
        /// </summary>
        void SetupImageInfos();
    };
}

#endif // !_TEXTURE_DESCRIPTOR_OBJECT_