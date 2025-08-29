#ifndef SceneIncluded
#define SceneIncluded

// File includes
#include "Managers/SceneManager.h"

namespace DDM
{
	class GameObject;
	class Camera;
	class LightComponent;

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

		void RenderSkyBox();

		void RenderDepth() const;

		void Render() const;

		// Render transparant object
		void RenderTransparancy() const;

		void OngGUI() const;

		void SetCamera(std::shared_ptr<Camera> pCamera);

		const std::shared_ptr<Camera> GetCamera() const;

		void SetLight(std::shared_ptr<LightComponent> pLight);

		const std::shared_ptr<LightComponent> GetLight() const;

		GameObject* GetSceneRoot();

	private:

		explicit Scene(const std::string& name);

		std::string m_Name;

		static unsigned int m_IdCounter;

		std::unique_ptr<GameObject> m_pSceneRoot{};

		std::shared_ptr<Camera> m_pActiveCamera{};

		std::unique_ptr<GameObject> m_pDefaultCamera{};
		std::shared_ptr<Camera> m_pDefaultCameraComponent{};


		std::shared_ptr<LightComponent> m_pActiveLight{};

		std::unique_ptr<GameObject> m_pDefaultLight{};
		std::shared_ptr<LightComponent> m_pDefaultLightComponent{};
	};
}

#endif // !SceneIncluded