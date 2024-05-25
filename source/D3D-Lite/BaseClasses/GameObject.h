#ifndef GameObjectIncluded
#define GameObjectIncluded
#include <string>
#include <memory>
#include <vector>

namespace D3D
{
	class Component;
	class TransformComponent;

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
		void SetParent(GameObject* pParent, bool worldPositionStays);

		void Init();

		void OnSceneLoad();

		void OnSceneUnload();

		void StartFrame();

		void EarlyUpdate();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void Render() const;

		void OnGUI();

		void Cleanup();

		void Destroy();

		bool ShouldDestroy() const { return m_ShouldDestroy; }

		template <class T>
		std::shared_ptr<T> GetComponent() const;
		
		template <class T>
		std::shared_ptr<T> AddComponent();

		template <class T>
		bool RemoveComponent();

		template <class T>
		bool HasComponent() const;
		
		GameObject* GetParent() const { return m_pParent; }

		const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return m_pChildren; }

		bool IsActive() const { return m_IsActive; }

		void SetActive(bool isActive) { m_IsActive = isActive; }

		bool ShouldShowImGui() const { return m_ShowImGui; }
		void SetShowImGui(bool showImGui) { m_ShowImGui = showImGui; }

		std::shared_ptr<TransformComponent> GetTransform() { return m_pTransform; }

		const std::string& GetName() const { return m_Name; }
		const std::string& GetTag() const { return m_Tag; }

	private:
		const std::string m_Name;
		const std::string m_Tag;

		bool m_IsActive{ true };
		bool m_ShowImGui{ false };
		bool m_ShouldDestroy{ false };

		GameObject* m_pParent{};
		std::vector<std::unique_ptr<GameObject>> m_pChildren{};
		std::vector<std::unique_ptr<GameObject>> m_pChildrenToAdd{};

		std::shared_ptr<TransformComponent> m_pTransform{};

		std::vector<std::shared_ptr<Component>> m_pComponents{};

		bool m_ShowGuiWindow{ true };
	};

	template<class T>
	inline std::shared_ptr<T> GameObject::GetComponent() const
	{
		if (!std::is_base_of<Component, T>())
		{
			return nullptr;
		}

		for (const auto& pComponent : m_pComponents)
		{
			if (auto pCastedComponent{ std::dynamic_pointer_cast<T>(pComponent) })
			{
				return pCastedComponent;
			}
		}

		return nullptr;
	}

	template<class T>
	inline std::shared_ptr<T> GameObject::AddComponent()
	{
		if (!std::is_base_of<Component, T>())
			return nullptr;
	
		if (HasComponent<T>())
		{
			return GetComponent<T>();
		}

		auto pComponent = std::make_shared<T>();

		pComponent->SetOwner(this);

		m_pComponents.push_back(pComponent);

		return pComponent;
	}

	template <class T>
	inline bool GameObject::RemoveComponent()
	{
		if (!std::is_base_of<Component, T>())
			return false;

		auto pComponent{ GetComponent<T>() };
		if (pComponent)
		{
			pComponent->Destroy();
			return true;
		}

		return false;
	}


	template<class T>
	inline bool GameObject::HasComponent() const
	{
		if (!std::is_base_of<Component, T>())
			return false;

		for (const auto& pComponent : m_pComponents)
		{
			if (std::dynamic_pointer_cast<T>(pComponent))
			{
				return true;
			}
		}
		return false;
	}

}

#endif // !GameObjectIncluded