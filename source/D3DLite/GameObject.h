#pragma once


namespace D3D
{
	class GameObject final
	{
	public:
		GameObject() : GameObject("UnNamed", "Default"){}
		GameObject(const std::string& name = "UnNamed", const std::string& tag = "Default") : m_Name{name}, m_Tag{tag} {}
		
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;

		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		GameObject* CreateNewObject(const std::string& name = "UnNamed", const std::string& tag = "Default");
		void RemoveAllChildren();

		void Init();

		void OnSceneLoad();

		void OnSceneUnload();

		void StartFrame();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void Render() const;

		void OnGUI();

		void Cleanup();

		void Destroy();

		bool ShouldDestroy() const { return m_ShouldDestroy; }

	private:
		const std::string m_Name;
		const std::string m_Tag;

		bool m_IsActive{ true };
		bool m_ShouldDestroy{ false };

		GameObject* m_pParent{};
		std::vector<std::unique_ptr<GameObject>> m_pChildren{};
		std::vector<std::unique_ptr<GameObject>> m_pChildrenToAdd{};

	};
}