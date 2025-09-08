// Camera.h


#ifndef CameraComponentIncluded
#define CameraComponentIncluded

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "DataTypes/Structs.h"
#include "Includes/VulkanIncludes.h"
#include "Components/Skybox.h"

namespace DDM
{
	class Transform;

	class Camera final : public Component
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		Camera();

		/// <summary>
		/// Destructor
		/// </summary>
		~Camera();

		/// <summary>
		/// Intialize function
		/// </summary>
		virtual void Init() override;

		/// <summary>
		/// Set FOV angle in radians
		/// </summary>
		/// <param name="angle: ">angle in radians</param>
		void SetFovAngleRad(float angle) { m_FovAngle = angle; }

		/// <summary>
		/// Set FOV angle in degrees
		/// </summary>
		/// <param name="angle: ">angle in degrees</param>
		void SetFovAngleDeg(float angle);

		/// <summary>
		/// Late update
		/// </summary>
		virtual void LateUpdate() override;

		// Update uniform buffer with camera transform
		// Parameters:
		//     buffer: reference to the uniform buffer object that needs updating
		//     extent: the extent of the swapchain

		/// <summary>
		/// Update uniform buffer with camera transform
		/// </summary>
		/// <param name="buffer: ">reference to the uniform buffer object to update</param>
		void UpdateUniformBuffer(UniformBufferObject& buffer);
		
		/// <summary>
		/// Render the skybox
		/// </summary>
		void RenderSkybox();

		/// <summary>
		/// Get the projection matrix
		/// </summary>
		/// <returns>Reference to projection matrix</returns>
		const glm::mat4& GetProjectionMatrix();

		/// <summary>
		/// Get the projection matrix
		/// </summary>
		/// <returns>Pointer to the projection matrix</returns>
		glm::mat4* GetProjectionMatrixPtr();

		/// <summary>
		/// Get the view matrix
		/// </summary>
		/// <returns>reference to the view matrix</returns>
		const glm::mat4& GetViewMatrix();

		/// <summary>
		/// Get the view matrix
		/// </summary>
		/// <returns>Pointer to the view matrix</returns>
		glm::mat4* GetViewMatrixPtr();
		
		virtual void OnSceneUnload() override;

	private:
		// Projection matrix
		glm::mat4 m_ProjectionMatrix{};

		// View matrix
		glm::mat4 m_ViewMatrix{};

		// Camera fov angle
		float m_FovAngle{};

		// Camera default fov angle
		const float m_DefaultAngleDegrees{ 90.f };

		// Pointer to skybox component
		std::shared_ptr<SkyBoxComponent> m_pSkyBox{};

		std::shared_ptr<Transform> m_pTransform{};

		/// <summary>
		/// Update the view matrix
		/// </summary>
		void UpdateViewMatrix();

		/// <summary>
		/// Update the projection matrix
		/// </summary>
		void UpdateProjectionMatrix();
	};
}
#endif // !CameraComponentIncluded
