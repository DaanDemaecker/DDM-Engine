// ResourceManager.h
// This class will serve as a singleton that allocates and loads resources like 3D models and textures
#ifndef ResourceManagerIncluded
#define ResourceManagerIncluded

// File includes
#include "Engine/Singleton.h"
#include "Vulkan/VulkanWrappers/Mesh.h"

// Standard library includes
#include <memory>
#include <string>

namespace DDM
{
	class Material;

	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		std::shared_ptr<DDM::Mesh> LoadMesh(std::string&& filePath);
		std::shared_ptr<DDM::Mesh> LoadMesh(std::string& filePath);

		std::shared_ptr<Material> GetDefaultMaterial() const;

	private:
		// Default constructor
		friend class Singleton<ResourceManager>;
		ResourceManager();

		std::shared_ptr<DDM::Mesh> m_pDefaultMesh{};

		std::shared_ptr<Material> m_pDefaultMaterial{};

		// Factory method to create Mesh instances
		std::shared_ptr<Mesh> CreateMesh(const std::string& filePath);
	};

}
#endif // !ResourceManagerIncluded
