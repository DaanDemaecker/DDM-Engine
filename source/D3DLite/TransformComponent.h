#ifndef TransformComponenetIncluded
#define TransformComponenetIncluded
#include "Component.h"

namespace D3D
{
	class TransformComponent final : public Component
	{
	public:
		//Location
		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }
		void SetLocalPosition(float x, float y, float z);
		void SetLocalPosition(const glm::vec3& pos);

		void Translate(float x, float y, float z);
		void Translate(const glm::vec3& dir);

		glm::vec3 GetWorldPosition();
		void SetWorldPosition(float x, float y, float z);
		void SetWorldPosition(const glm::vec3& pos);

		void SetPositionDirtyFlag();

		//Rotation
		const glm::vec3& GetLocalRotation() const { return m_LocalRotation; }
		void SetLocalRotation(float x, float y, float z);
		void SetLocalRotation(const glm::vec3& rot);

		void Rotate(float x, float y, float z);
		void Rotate(const glm::vec3& dir);

		glm::vec3 GetWorldRotation();
		void SetWorldRotation(float x, float y, float z);
		void SetWorldRotation(const glm::vec3& rot);

		void SetRotationDirtyFlag();

		//Scale
		const glm::vec3& GetLocalScale() const { return m_LocalRotation; }
		void SetLocalScale(float x, float y, float z);
		void SetLocalScale(const glm::vec3& scale);

		glm::vec3 GetWorldScale();
		void SetWorldScale(float x, float y, float z);
		void SetWorldScale(const  glm::vec3 scale);

		void SetScaleDirtyFlag();

	private:
		glm::vec3 m_LocalPosition{};
		glm::vec3 m_ParentWorldPosition{};
		bool m_PositionDF{ true };

		glm::vec3 m_LocalRotation{};
		glm::vec3 m_ParentWorldRotation{};
		bool m_RotationDF{ true };

		glm::vec3 m_LocalScale{1, 1, 1};
		glm::vec3 m_ParentWorldScale{};
		bool m_ScaleDF{ true };

		void NormalizeRotation();
	};
}
#endif // !TransformComponenetIncluded
