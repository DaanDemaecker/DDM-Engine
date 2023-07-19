#ifndef SceneManagerIncluded
#define SceneManagerIncluded
#include "Singleton.h"

namespace D3D
{
	class Scene;

	class SceneManager : public Singleton<SceneManager>
	{
	public:
		void EndProgram();

		std::shared_ptr<Scene> CreateScene(const std::string& name);
		void DeleteScene(const std::string& name);
		std::shared_ptr<Scene> GetScene(const std::string& name);

		void NextScene();
		void PreviousScene();
		void SetActiveScene(const std::string& name);

		std::shared_ptr<Scene> GetActiveScene() { return m_ActiveScene; }

		void SetActiveScene(std::shared_ptr<Scene> scene);
		void SetActiveScene(std::string& name);

		void StartFrame();

		void Update();
		void FixedUpdate();

		void LateUpdate();

		void OnGui();

		void Render() const;

		void Cleanup();

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;

		std::vector<std::shared_ptr<Scene>> m_pScenes{};

		std::shared_ptr<Scene> m_ActiveScene{}, m_NextActiveScene{};
	};
}
#endif // !SceneManagerIncluded