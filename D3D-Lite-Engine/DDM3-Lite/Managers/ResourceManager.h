// ResourceManager.h
// This class will serve as a singleton that allocates and loads resources like 3D models and textures
#ifndef ResourceManagerIncluded
#define ResourceManagerIncluded

// File includes
#include "Engine/Singleton.h"
#include "DataTypes/Mesh.h"

// Standard library includes
#include <memory>
#include <string>

namespace DDM3
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		std::shared_ptr<DDM3::Mesh> LoadMesh(std::string&& filePath);
		std::shared_ptr<DDM3::Mesh> LoadMesh(std::string& filePath);

	private:
		// Default constructor
		friend class Singleton<ResourceManager>;
		ResourceManager();

		std::shared_ptr<DDM3::Mesh> m_pDefaultMesh{};


		// Factory method to create Mesh instances
		std::shared_ptr<Mesh> CreateMesh(const std::string& filePath);
	};

}
#endif // !ResourceManagerIncluded
