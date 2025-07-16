// SceneManager.h
// This singleton will hold, manage and handle all scenes


#ifndef SceneManagerIncluded
#define SceneManagerIncluded

// File includes
#include "Engine/Singleton.h"

// Standard library includes
#include <memory>
#include <iostream>
#include <vector>

namespace DDM3
{
	// Class forward declarations
	class Scene;
	class CameraComponent;
	class DirectionalLightComponent;

	class SceneManager : public Singleton<SceneManager>
	{
	public:
		// Clears all scenes
		void EndProgram();

		// Create a new scene
		// Parameters:
		//     name: the name of the new scene
		std::shared_ptr<Scene> CreateScene(const std::string& name);

		// Get a pointer to a scene
		// Parameters:
		//     name: the name of the requested scene
		std::shared_ptr<Scene> GetScene(const std::string& name);

		// Delete a scene
		// Parameters:
		//     name: the name of the scene to be deleted
		void DeleteScene(const std::string& name);

		// Go to the next scene
		void NextScene();

		// Go to the previous scene
		void PreviousScene();

		// Set a specific scene active
		// Parameters:
		//     name: the name of the requested scene
		void SetActiveScene(const std::string& name);

		// Get a pointer to the active scene
		std::shared_ptr<Scene> GetActiveScene();

		// Set a specific scene active
		// Parameters:
		//     scene: pointer to the requested scene
		void SetActiveScene(std::shared_ptr<Scene> scene);

		// Set a specific scene active
		// Parameters:
		//     name: name of the requested scene
		void SetActiveScene(std::string& name);

		// Function that sets up the next frame
		void StartFrame();

		// Early update function
		void EarlyUpdate();

		// Update function
		void Update();
		
		// Fixed update function
		void FixedUpdate();

		// Late update function
		void LateUpdate();
		
		// Post update function
		void PostUpdate();

		// OnGui function
		void OnGui();

		void PrepareRender();

		// Render the skybox
		void RenderSkybox();

		void RenderDepth() const;

		// Render everything
		void Render() const;

		// Render transparant object
		void RenderTransparancy() const;

		// Clean up the frame
		void Cleanup();

		// Get a pointer to the active camera
		const std::shared_ptr<CameraComponent> GetCamera() const;

		// Get a pointer to the active global light
		const std::shared_ptr<DirectionalLightComponent> GetGlobalLight() const;

	private:
		// Default constructor
		friend class Singleton<SceneManager>;
		SceneManager() = default;

		// Vector of all the loaded scenes
		std::vector<std::shared_ptr<Scene>> m_pScenes{};

		// Pointer to the current active scene
		std::shared_ptr<Scene> m_ActiveScene{};

		// Pointer to the next active scene
		std::shared_ptr<Scene> m_NextActiveScene{};
	};
}
#endif // !SceneManagerIncluded