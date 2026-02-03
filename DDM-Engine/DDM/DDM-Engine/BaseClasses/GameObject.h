// GameObject.h
// This class serves as a base game object
// It can contain multiple components and child game objects

#ifndef _DDM_GAMEOBJECT_
#define _DDM_GAMEOBJECT_

// Standard library includes
#include <string>
#include <memory>
#include <vector>

namespace DDM
{
	// Class forward declarations
	class Component;
	class Transform;

	class GameObject final
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="name: ">The name of the game object</param>
		/// <param name="tag: ">The tag associated with the game object</param>
		GameObject(const std::string& name = "UnNamed", const std::string& tag = "Default") : m_Name{name}, m_Tag{tag} {}
		
		/// <summary>
		/// Destructor
		/// </summary>
		~GameObject();

		// Rule of five
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;

		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		/// <summary>
		/// Create a new child game object
		/// </summary>
		/// <param name="name: ">The name of the game object</param>
		/// <param name="tag: ">The tag associated with the game object</param>
		/// <returns>Pointer to the new gameobject</returns>
		GameObject* CreateNewObject(const std::string& name = "UnNamed", const std::string& tag = "Default");
		
		/// <summary>
		/// Remove all child objects
		/// </summary>
		void RemoveAllChildren();

		/// <summary>
		/// Set a new parent
		/// </summary>
		/// <param name="pParent: ">pointer to new parent</param>
		/// <param name="worldPositionStays: ">boolean indicating wether object should keep world or relative position</param>
		void SetParent(GameObject* pParent, bool worldPositionStays = true);

		/// <summary>
		/// Initialize object
		/// </summary>
		void Init();

		/// <summary>
		/// Gets called when scene is loaded
		/// </summary>
		void OnSceneLoad();

		/// <summary>
		/// Gets called when scene is unloaded
		/// </summary>
		void OnSceneUnload();

		/// <summary>
		/// Gets called at the beginning of the frame, used to initialize children
		/// </summary>
		void StartFrame();

		/// <summary>
		/// Gets called at the start of the update loop
		/// </summary>
		void EarlyUpdate();

		/// <summary>
		/// Main update function
		/// </summary>
		void Update();

		/// <summary>
		/// Physics update function
		/// </summary>
		void FixedUpdate();

		/// <summary>
		/// Gets called at the end of the update loop
		/// </summary>
		void LateUpdate();

		/// <summary>
		/// Renders the depth prepass
		/// </summary>
		void RenderDepth() const;

		/// <summary>
		/// Main render fucntion
		/// </summary>
		void Render() const;

		/// <summary>
		/// Render transparancy pass
		/// </summary>
		void RenderTransparancy() const;

		/// <summary>
		/// Renders the ImGui elements
		/// </summary>
		void OnGUI();

		/// <summary>
		/// Gets called at the end of the update loop, used to delete components and children marked for destruction
		/// </summary>
		void PostUpdate();

		/// <summary>
		/// Destroy this game object and it's attached components and children
		/// </summary>
		void Destroy();

		/// <summary>
		/// Check if game object is marked for destruction
		/// </summary>
		/// <returns>boolean indicating if object is marked for destruction</returns>
		bool ShouldDestroy() const { return m_ShouldDestroy; }

		/// <summary>
		/// Get component of type T
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns></returns>
		template <class T>
		std::shared_ptr<T> GetComponent() const;
		
		/// <summary>
		/// Add a component of type T
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns></returns>
		template <class T>
		std::shared_ptr<T> AddComponent();

		/// <summary>
		/// Remove component of type T
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns></returns>
		template <class T>
		bool RemoveComponent();

		/// <summary>
		/// Check if object has component of type T
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns></returns>
		template <class T>
		bool HasComponent() const;
		
		/// <summary>
		/// Get the parent of this object
		/// </summary>
		/// <returns>Pointer to the parent</returns>
		GameObject* GetParent() const { return m_pParent; }

		/// <summary>
		/// Get a list of all children of this object
		/// </summary>
		/// <returns>Reference to list of all children</returns>
		const std::vector<std::unique_ptr<GameObject>>& GetChildren() const { return m_pChildren; }

		/// <summary>
		/// Check if this object is active
		/// </summary>
		/// <returns>Boolean indicating if object is active</returns>
		bool IsActive() const { return m_IsActive; }

		/// <summary>
		/// Set active mode of this object
		/// </summary>
		/// <param name="isActive: ">new active mode</param>
		void SetActive(bool isActive) { m_IsActive = isActive; }

		/// <summary>
		/// Check if object should render its ImGui elements
		/// </summary>
		/// <returns>boolean indicating wether object should render ImGui</returns>
		bool ShouldShowImGui() const { return m_ShowImGui; }

		/// <summary>
		/// Set new render ImGui mode
		/// </summary>
		/// <param name="showImGui: ">new render ImGui mode</param>
		void SetShowImGui(bool showImGui) { m_ShowImGui = showImGui; }

		/// <summary>
		/// Get the transform component
		/// </summary>
		/// <returns>Pointer to the transform component</returns>
		std::shared_ptr<Transform> GetTransform() { return m_pTransform; }

		/// <summary>
		/// Get the name of this object
		/// </summary>
		/// <returns>Reference to name of this object</returns>
		const std::string& GetName() const { return m_Name; }

		/// <summary>
		/// Get the tag of this object
		/// </summary>
		/// <returns>Reference to tag of this object</returns>
		const std::string& GetTag() const { return m_Tag; }

	private:
		// Name of this object
		const std::string m_Name;
		
		// Tag of this object
		const std::string m_Tag;

		// Indicates wether this object is active or not
		bool m_IsActive{ true };

		// Indicates if object should render its ImGui elements
		bool m_ShowImGui{ false };

		// Indicates if the ImGui window is open
		bool m_ShowGuiWindow{ true };

		// Indicates if object should be destroyed
		bool m_ShouldDestroy{ false };

		// Pointer to parent and list of children
		GameObject* m_pParent{};

		// List of children
		std::vector<std::unique_ptr<GameObject>> m_pChildren{};

		/// List of children to add next frame
		std::vector<std::unique_ptr<GameObject>> m_pChildrenToAdd{};

		// Pointer to transform component
		std::shared_ptr<Transform> m_pTransform{};

		// List of components attached to this object
		std::vector<std::shared_ptr<Component>> m_pComponents{};
	};

	template<class T>
	inline std::shared_ptr<T> GameObject::GetComponent() const
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
		{
			return nullptr;
		}

		// Loop through all components and try to cast them to T
		for (const auto& pComponent : m_pComponents)
		{
			// If component of type T is found, return it
			if (auto pCastedComponent{ std::dynamic_pointer_cast<T>(pComponent) })
			{
				return pCastedComponent;
			}
		}
		
		// If no component of type T is found, return nullptr
		return nullptr;
	}

	template<class T>
	inline std::shared_ptr<T> GameObject::AddComponent()
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
			return nullptr;
	
		// Check if component of type T already exists
		if (HasComponent<T>())
		{
			return GetComponent<T>();
		}

		// Create new component of type T
		auto pComponent = std::make_shared<T>();

		// Set owner to new component
		pComponent->SetOwner(this);

		// Add component to list
		m_pComponents.push_back(pComponent);

		// Call init function on component
		pComponent->Init();

		// Return pointer to new component
		return pComponent;
	}

	template <class T>
	inline bool GameObject::RemoveComponent()
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
			return false;

		// Get component of type T
		auto pComponent{ GetComponent<T>() };
		
		// If component exists, destroy it and return true
		if (pComponent)
		{
			pComponent->Destroy();
			return true;
		}

		// If component does not exist, return false
		return false;
	}


	template<class T>
	inline bool GameObject::HasComponent() const
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
			return false;

		// Loop through all components and try to cast them to T
		for (const auto& pComponent : m_pComponents)
		{
			// If component of type T is found, return true
			if (std::dynamic_pointer_cast<T>(pComponent))
			{
				return true;
			}
		}

		// If no component of type T is found, return false
		return false;
	}

}

#endif // !_DDM_GAMEOBJECT_