#ifndef SceneIncluded
#define SceneIncluded
#include "SceneManager.h"


namespace D3D
{
	class Model;
	class GameObject;

	class Scene final
	{
		friend std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name);
	public:
		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::string& GetName() const { return m_Name; }

		GameObject* CreateGameObject(const std::string& name = "unNamed");

		void OnSceneLoad();

		void OnSceneUnload();

		void StartFrame();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void Render() const;

		void OngGUI() const;

		void Cleanup();

	private:

		explicit Scene(const std::string& name);

		std::string m_Name;

		std::vector<std::unique_ptr<Model>> m_pModels{};

		static unsigned int m_IdCounter;

		std::unique_ptr<GameObject> m_pSceneRoot{};
	};
}

#endif // !SceneIncluded