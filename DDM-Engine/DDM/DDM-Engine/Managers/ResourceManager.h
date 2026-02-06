// ResourceManager.h
// This class will serve as a singleton that allocates and loads resources like 3D models and textures
#ifndef ResourceManagerIncluded
#define ResourceManagerIncluded

// File includes
#include "DDM-Engine/Engine/Singleton.h"
#include "DDM-Engine/Vulkan/VulkanWrappers/Mesh.h"

// Export include
#include "DDM-Engine/Export.h"

// Standard library includes
#include <memory>
#include <string>

namespace DDM
{
	class Material;

	class DDM_API ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		std::shared_ptr<DDM::Mesh> LoadMesh(std::string&& filePath);
		std::shared_ptr<DDM::Mesh> LoadMesh(std::string& filePath);

		std::shared_ptr<Material> GetDefaultMaterial() const;

		void EndProgram();
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
