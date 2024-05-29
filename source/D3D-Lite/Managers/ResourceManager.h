// ResourceManager.h
// This class will serve as a singleton that allocates and loads resources like 3D models and textures
#ifndef ResourceManagerIncluded
#define ResourceManagerIncluded

// File includes
#include "../Engine/Singleton.h"

// Standard library includes

namespace D3D
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:

	private:
		// Default constructor
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;

	};

}
#endif // !ResourceManagerIncluded
