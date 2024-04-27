// ModelLoader.h
// This class will serve as the baseclass of all model loaders
// This class will be an abstract class as an instance of the class shouldn't be allowed
// This file will also hold the default model loader class

namespace D3D
{
	class ModelLoader
	{
	public:
		ModelLoader() = default;
		virtual ~ModelLoader() = default;

		ModelLoader(ModelLoader& other) = delete;
		ModelLoader(ModelLoader&& other) = delete;

		ModelLoader& operator=(ModelLoader& other) = delete;
		ModelLoader& operator=(ModelLoader&& other) = delete;

	};


	class DefaultModelLoader final : public ModelLoader
	{
	public:
		DefaultModelLoader() = default;
		virtual ~DefaultModelLoader() override = default;

	};
}