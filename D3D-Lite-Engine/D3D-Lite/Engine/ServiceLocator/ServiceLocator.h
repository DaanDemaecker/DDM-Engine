// ServiceLocator.h
// This class will hold and manage services like soundsystems and model loaders

#ifndef ServiceLocatorIncluded
#define ServiceLocatorIncluded

// File includes
#include "ModelLoader/ModelLoader.h"

// Standard library includes
#include <memory>

namespace D3D
{
	class ServiceLocator final
	{
	public:
		ServiceLocator() = default;
		~ServiceLocator() = default;

		ServiceLocator(ServiceLocator& other) = delete;
		ServiceLocator(ServiceLocator&& other) = delete;
		ServiceLocator operator=(ServiceLocator& other) = delete;
		ServiceLocator operator=(ServiceLocator&& other) = delete;

		static ModelLoader& GetModelLoader();
		static void RegisterModelLoader(std::unique_ptr<ModelLoader> modelLoader);

	private:
		static std::unique_ptr<ModelLoader> m_pModelLoaderInstance;
		static std::unique_ptr<DefaultModelLoader> m_pDefaultModelLoaderInstance;

	};
}
#endif // !ServiceLocatorIncluded