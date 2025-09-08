// Component.h
// This class will serve as the base for any component created

#ifndef _COMPONENT_
#define _COMPONENT_

// File includes
#include "GameObject.h"

// Standard library includes
#include <memory>

namespace DDM
{
	// Class forward declarations
	class Transform;

	class Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		Component() = default;

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~Component() = default;

		// Rule of five
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		/// <summary>
		/// Gets called right after creating component
		/// </summary>
		virtual void Init(){}

		/// <summary>
		/// Gets called when scene is loaded
		/// </summary>
		virtual void OnSceneLoad() {}

		/// <summary>
		/// Gets called when scene is unloaded
		/// </summary>
		virtual void OnSceneUnload() {}

		/// <summary>
		/// Gets called at the start of the update loop
		/// </summary>
		virtual void EarlyUpdate(){}

		/// <summary>
		/// Main update function
		/// </summary>
		virtual void Update() {}

		/// <summary>
		/// Physics update function
		/// </summary>
		virtual void FixedUpdate() {}

		/// <summary>
		/// Gets called at the end of the update loop
		/// </summary>
		virtual void LateUpdate() {}

		/// <summary>
		/// Renders the depth prepass
		/// </summary>
		virtual void RenderDepth() {}

		/// <summary>
		/// Main render fucntion
		/// </summary>
		virtual void Render() {}

		/// <summary>
		/// Render transparancy pass
		/// </summary>
		virtual void RenderTransparancy() {}

		/// <summary>
		/// Renders the ImGui elements
		/// </summary>
		virtual void OnGUI() {}

		/// <summary>
		/// Destroy this component
		/// </summary>
		virtual void Destroy() { m_ShouldDestroy = true; }

		/// <summary>
		/// Checks if component should be destroyed
		/// </summary>
		/// <returns>Boolean indicating if destruction is necessary</returns>
		bool ShouldDestroy() { return m_ShouldDestroy; }

		/// <summary>
		/// Get a component of type T from the gameobject this component is attached to
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns>Shared pointer to component or nullptr if not found</returns>
		template <class T>
		std::shared_ptr<T> GetComponent() const;
		
		/// <summary>
		/// Check if the gameobject this component is attached to has a component of type T
		/// </summary>
		/// <typeparam name="T">Type of component</typeparam>
		/// <returns>Booleans indicating if gameobject has this component</returns>
		template <class T>
		bool HasComponent() const;

		/// <summary>
		/// Get the owning game object of this component
		/// </summary>
		/// <returns>Pointer to game object</returns>
		GameObject* GetOwner() const { return m_pOwner; }

		/// <summary>
		/// Get the transform component of the owning game object
		/// </summary>
		/// <returns>Pointer to the transform component</returns>
		std::shared_ptr<Transform> GetTransform() { return m_pOwner->GetTransform(); }

		/// <summary>
		/// Check if component is active
		/// </summary>
		/// <returns>Boolean indicating if component is active</returns>
		bool IsActive() const { return m_IsActive; }

		/// <summary>
		/// Set active mode of component
		/// </summary>
		/// <param name="isActive: ">new active mode</param>
		void SetActive(bool isActive) { m_IsActive = isActive; }

		/// <summary>
		/// Check if component should render its ImGui elements
		/// </summary>
		/// <returns>Boolean indicating if component should render its ImGui elements</returns>
		bool ShouldShowImGui() const { return m_ShowImGui; }

		/// <summary>
		/// Set active mode of ImGui rendering for this component
		/// </summary>
		/// <param name="showImGui: ">new active mode of ImGui rendering</param>
		void SetShowImGui(bool showImGui) { m_ShowImGui = showImGui; }

	private:
		// Friend class declarations
		template <class T>
		friend std::shared_ptr<T> GameObject::AddComponent();

		// Indicates if component should be destroyed
		bool m_ShouldDestroy{ false };

		// Indicates if component is active
		bool m_IsActive{ true };

		// Indicates if component should render its ImGui elements
		bool m_ShowImGui{ false };

		// Pointer to the game object that owns this component
		GameObject* m_pOwner{nullptr};

		/// <summary>
		/// Set new owner of this object
		/// </summary>
		/// <param name="pOwner: ">new object</param>
		void SetOwner(GameObject* pOwner) { m_pOwner = pOwner; }
	};

	template <class T>
	inline std::shared_ptr<T> Component::GetComponent() const
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
			return nullptr;

		// Check if owner is valid
		if (m_pOwner == nullptr)
			return nullptr;

		// Return component from owner
		return m_pOwner->GetComponent<T>();
	}

	template <class T>
	inline bool Component::HasComponent() const
	{
		// Check if T is derived from Component
		if (!std::is_base_of<Component, T>())
			return false;

		// Check if owner is valid
		if (m_pOwner == nullptr)
			return false;

		// Return if owner has component
		return m_pOwner->HasComponent<T>();
	}
}
#endif // !_COMPONENT_
