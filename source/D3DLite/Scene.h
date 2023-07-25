#ifndef SceneIncluded
#define SceneIncluded
#include "SceneManager.h"


namespace D3D
{
	class GameObject;
	class CameraComponent;

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

		void SetCamera(std::shared_ptr<CameraComponent> pCamera);

		const std::shared_ptr<CameraComponent> GetCamera() const;

	private:

		explicit Scene(const std::string& name);

		std::string m_Name;

		static unsigned int m_IdCounter;

		std::unique_ptr<GameObject> m_pSceneRoot{};

		std::shared_ptr<CameraComponent> m_pActiveCamera{};

		std::unique_ptr<GameObject> m_pDefaultCamera{};
		std::shared_ptr<CameraComponent> m_pDefaultCameraComponent{};
	};
}

#endif // !SceneIncluded