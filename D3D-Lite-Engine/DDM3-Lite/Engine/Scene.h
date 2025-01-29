#ifndef SceneIncluded
#define SceneIncluded

// File includes
#include "Managers/SceneManager.h"

namespace DDM3
{
	class GameObject;
	class CameraComponent;
	class DirectionalLightComponent;

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

		void EarlyUpdate();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void PostUpdate();

		void PrepareRender();

		void RenderSkyBox();

		void Render() const;

		void OngGUI() const;

		void Cleanup();

		void SetCamera(std::shared_ptr<CameraComponent> pCamera);

		const std::shared_ptr<CameraComponent> GetCamera() const;

		void SetLight(std::shared_ptr<DirectionalLightComponent> pLight);

		const std::shared_ptr<DirectionalLightComponent> GetLight() const;

	private:

		explicit Scene(const std::string& name);

		std::string m_Name;

		static unsigned int m_IdCounter;

		std::unique_ptr<GameObject> m_pSceneRoot{};

		std::shared_ptr<CameraComponent> m_pActiveCamera{};

		std::unique_ptr<GameObject> m_pDefaultCamera{};
		std::shared_ptr<CameraComponent> m_pDefaultCameraComponent{};


		std::shared_ptr<DirectionalLightComponent> m_pActiveLight{};

		std::unique_ptr<GameObject> m_pDefaultLight{};
		std::shared_ptr<DirectionalLightComponent> m_pDefaultLightComponent{};
	};
}

#endif // !SceneIncluded