#ifndef CameraComponentIncluded
#define CameraComponentIncluded

// File includes
#include "../BaseClasses/Component.h"
#include "../DataTypes/Structs.h"
#include "../Includes/VulkanIncludes.h"
#include "../Components/SkyBoxComponent.h"

namespace DDM3
{

	class CameraComponent final : public Component
	{
	public:
		CameraComponent();
		~CameraComponent() = default;

		void SetFovAngle(float angle) { m_FovAngle = angle; }
		void SetFovAngleDegrees(float angle);

		virtual void LateUpdate() override;
		

		float GetFovAngle() const { return m_FovAngle; }

		// Update uniform buffer with camera transform
		// Parameters:
		//     buffer: reference to the uniform buffer object that needs updating
		//     extent: the extent of the swapchain
		void UpdateUniformBuffer(UniformBufferObject& buffer, VkExtent2D extent);
		
		void RenderSkybox();

		glm::mat4& GetProjectionMatrix();

		glm::mat4* GetProjectionMatrixPointer();

		glm::mat4& GetViewMatrix();

		glm::mat4* GetViewMatrixPointer();

	private:
		glm::mat4 m_ProjectionMatrix{};
		glm::mat4 m_ViewMatrix{};

		bool m_ShouldUpdateProjection{ true };

		float m_FovAngle{};
		const float m_DefaultAngleDegrees{ 90.f };

		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

		std::shared_ptr<SkyBoxComponent> m_pSkyBox{};
	};
}
#endif // !CameraComponentIncluded
