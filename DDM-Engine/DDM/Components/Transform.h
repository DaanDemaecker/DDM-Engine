// Transform.h
// THis class will represent the transform of the object
// The transform holds and handles rotation, position and scale

#ifndef _TRANSFORM_
#define _TRANSFORM_

// Parent include
#include "BaseClasses/Component.h"

// File includes
#include "Includes/GLMIncludes.h"

namespace DDM
{
	struct TransformPod
	{
		glm::vec3 pos{};
		glm::quat rot{};
		glm::vec3 scale{};
	};

	class Transform final : public Component
	{
	public:
		/// <summary>
		/// Default constructor
		/// </summary>
		Transform() = default;

		/// <summary>
		/// Default destructor
		/// </summary>
		~Transform() = default;

		// Rule of five
		Transform(Transform&) = delete;
		Transform(Transform&&) = delete;

		Transform& operator=(Transform&) = delete;
		Transform& operator=(Transform&&) = delete;

		/// <summary>
		/// OnGUi function
		/// </summary>
		virtual void OnGUI() override;

		// -------------------
		// Location
		// -------------------

		/// <summary>
		/// Get the local position of the object
		/// </summary>
		/// <returns>Reference to local position</returns>
		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }

		/// <summary>
		/// Set local position
		/// </summary>
		/// <param name="x: ">new x value</param>
		/// <param name="y: ">new y value</param>
		/// <param name="z: ">new z value</param>
		void SetLocalPosition(float x, float y, float z);

		/// <summary>
		/// Set local position
		/// </summary>
		/// <param name="pos: ">new local position</param>
		void SetLocalPosition(const glm::vec3& pos);

		/// <summary>
		/// Set local position
		/// </summary>
		/// <param name="pos: ">new local position</param>
		void SetLocalPosition(const glm::vec3&& pos);

		/// <summary>
		/// Translate the object a given amount
		/// </summary>
		/// <param name="x: ">translate x value</param>
		/// <param name="y: ">translate y value</param>
		/// <param name="z: ">translate z value</param>
		void Translate(float x, float y, float z);

		/// <summary>
		/// Translate the object a given amount
		/// </summary>
		/// <param name="dir: ">translate value</param>
		void Translate(const glm::vec3& dir);

		/// <summary>
		/// Translate the object a given amount
		/// </summary>
		/// <param name="dir: ">translate value</param>
		void Translate(const glm::vec3&& dir);

		/// <summary>
		/// Get the world position of the object
		/// </summary>
		/// <returns>World position by value</returns>
		glm::vec3 GetWorldPosition();

		/// <summary>
		/// Set the world position of the object
		/// </summary>
		/// <param name="x: ">new x value of worldpos</param>
		/// <param name="y: ">new y value of worldpos</param>
		/// <param name="z: ">new z value of worldpos</param>
		void SetWorldPosition(float x, float y, float z);

		/// <summary>
		/// Set the world position of the object
		/// </summary>
		/// <param name="pos: ">new value of the world position</param>
		void SetWorldPosition(const glm::vec3& pos);

		/// <summary>
		/// Set the world position of the object
		/// </summary>
		/// <param name="pos: ">new value of the world position</param>
		void SetWorldPosition(const glm::vec3&& pos);

		/// <summary>
		/// Set the dirty flag for the parent position
		/// </summary>
		void SetPositionDirtyFlag() const;

		
		// -------------------
		// Rotation
		// -------------------
		
		/// <summary>
		/// Get the local rotation quaternion
		/// </summary>
		/// <returns>Reference to local rotation quaternion</returns>
		const glm::quat& GetLocalRotation() const { return m_LocalRotation; }

		/// <summary>
		/// Get local rotation in euler angles
		/// </summary>
		/// <returns>Local rotation euler angles</returns>
		const glm::vec3 GetLocalRotationEuler() const;
		
		/// <summary>
		/// Set local rotation in euler angles
		/// </summary>
		/// <param name="x: ">new x value for euler angles</param>
		/// <param name="y: ">new y value for euler angles</param>
		/// <param name="z: ">new z value for euler angles</param>
		void SetLocalRotation(float x, float y, float z);

		/// <summary>
		/// Set local rotation in euler angles
		/// </summary>
		/// <param name="rot: ">new value for euler angles</param>
		void SetLocalRotation(const glm::vec3& rot);

		/// <summary>
		/// Set local rotation in euler angles
		/// </summary>
		/// <param name="rot: ">new value for euler angles</param>
		void SetLocalRotation(const glm::vec3&& rot);

		/// <summary>
		/// Set local rotation as quaternion
		/// </summary>
		/// <param name="rot: ">new value for rotation</param>
		void SetLocalRotation(const glm::quat& rot);

		/// <summary>
		/// Set local rotation as quaternion
		/// </summary>
		/// <param name="rot: ">new value for rotation</param>
		void SetLocalRotation(const glm::quat&& rot);

		/// <summary>
		/// Rotate the object over an axis
		/// </summary>
		/// <param name="axis: ">the axis over which to rotate</param>
		/// <param name="angle: ">angle to rotate in radians</param>
		void Rotate(glm::vec3& axis, float angle);

		/// <summary>
		/// Rotate the object over an axis
		/// </summary>
		/// <param name="axis: ">the axis over which to rotate</param>
		/// <param name="angle: ">angle to rotate in radians</param>
		void Rotate(glm::vec3&& axis, float angle);

		/// <summary>
		/// Rotate the object over an axis
		/// </summary>
		/// <param name="axis: ">the axis over which to rotate</param>
		/// <param name="angle: ">angle to rotate in degrees</param>
		void RotateDeg(glm::vec3& axis, float angle);

		/// <summary>
		/// Rotate the object over an axis
		/// </summary>
		/// <param name="axis: ">the axis over which to rotate</param>
		/// <param name="angle: ">angle to rotate in degrees</param>
		void RotateDeg(glm::vec3&& axis, float angle);

		/// <summary>
		/// Get world rotation
		/// </summary>
		/// <returns>Quaternion signifying world rotation</returns>
		glm::quat GetWorldRotation();

		/// <summary>
		/// Set the world rotation in euler angles
		/// </summary>
		/// <param name="x: ">new x value of rotation</param>
		/// <param name="y: ">new y value of rotation</param>
		/// <param name="z: ">new z value of rotation</param>
		void SetWorldRotation(float x, float y, float z);

		/// <summary>
		/// Set the world rotation in euler angles
		/// </summary>
		/// <param name="x: ">new value of rotation</param>
		void SetWorldRotation(const glm::vec3& rot);

		/// <summary>
		/// Set the world rotation in euler angles
		/// </summary>
		/// <param name="x: ">new value of rotation</param>
		void SetWorldRotation(const glm::vec3&& rot);

		/// <summary>
		/// Set the world rotation as a quaternion
		/// </summary>
		/// <param name="rot">new valu of rotation</param>
		void SetWorldRotation(const glm::quat& rot);

		/// <summary>
		/// Set the world rotation as a quaternion
		/// </summary>
		/// <param name="rot">new valu of rotation</param>
		void SetWorldRotation(const glm::quat&& rot);


		/// <summary>
		/// Set the dirty flag for the rotation
		/// </summary>
		void SetRotationDirtyFlag() const;

		// -------------------
		// Scale
		// -------------------
		
		/// <summary>
		/// Get local scale
		/// </summary>
		/// <returns>reference to local scene</returns>
		const glm::vec3& GetLocalScale() const { return m_LocalScale; }

		/// <summary>
		/// Set local scale
		/// </summary>
		/// <param name="x: ">new x value for scale</param>
		/// <param name="y: ">new y value for scale</param>
		/// <param name="z: ">new z value for scale</param>
		void SetLocalScale(float x, float y, float z);

		/// <summary>
		/// Set local scale
		/// </summary>
		/// <param name="scale: ">new value for scale</param>
		void SetLocalScale(const glm::vec3& scale);

		/// <summary>
		/// Set local scale
		/// </summary>
		/// <param name="scale: ">new value for scale</param>
		void SetLocalScale(const glm::vec3&& scale);


		/// <summary>
		/// Get the world scale
		/// </summary>
		/// <returns>World scale</returns>
		glm::vec3 GetWorldScale();

		/// <summary>
		/// Set world scale
		/// </summary>
		/// <param name="x: ">new x value for scale</param>
		/// <param name="y: ">new y value for scale</param>
		/// <param name="z: ">new z value for scale</param>
		void SetWorldScale(float x, float y, float z);

		/// <summary>
		/// Set world scale
		/// </summary>
		/// <param name="scale: ">new value for scale</param>
		void SetWorldScale(const glm::vec3& scale);

		/// <summary>
		/// Set world scale
		/// </summary>
		/// <param name="scale: ">new value for scale</param>
		void SetWorldScale(const glm::vec3&& scale);

		/// <summary>
		/// Set dirty flag for scale
		/// </summary>
		void SetScaleDirtyFlag() const;


		/// <summary>
		/// Get the forward direction of the object
		/// </summary>
		/// <returns>Forward vector</returns>
		glm::vec3 GetForward();

		/// <summary>
		/// Get the up direction of the object
		/// </summary>
		/// <returns>Up vector</returns>
		glm::vec3 GetUp();

		/// <summary>
		/// Get right direction of the object
		/// </summary>
		/// <returns>Right vector</returns>
		glm::vec3 GetRight();



		// -------------------
		// File handling
		// -------------------

		/// <summary>
		/// Write the contents of the component to a file
		/// </summary>
		/// <param name="fileName:">Path to the file to write to</param>
		/// <returns>Boolean indicating if writing was succesful</returns>
		bool WriteToFile(std::string& fileName);

		/// <summary>
		/// Write the contents of the component to a file
		/// </summary>
		/// <param name="fileName:">Path to the file to write to</param>
		/// <returns>Boolean indicating if writing was succesful</returns>
		bool WriteToFile(std::string&& fileName);

		/// <summary>
		/// Reads the contents of the comonent from a file
		/// </summary>
		/// <param name="fileName: ">Path to the file to read from</param>
		/// <returns>Boolean indicating if reading was succesful</returns>
		bool ReadFromFile(std::string& fileName);

		/// <summary>
		/// Reads the contents of the comonent from a file
		/// </summary>
		/// <param name="fileName: ">Path to the file to read from</param>
		/// <returns>Boolean indicating if reading was succesful</returns>
		bool ReadFromFile(std::string&& fileName);

	private:

		// -------------------
		// Position
		// -------------------
		
		// Position relative to parent
		glm::vec3 m_LocalPosition{};

		// World position of parent
		glm::vec3 m_ParentWorldPosition{};

		// Position dirty flag
		bool m_PositionDF{ true };

		// -------------------
		// Rotation
		// -------------------

		// Rotation relative to parent
		glm::quat m_LocalRotation{ glm::identity<glm::quat>() };

		// World rotation of parent
		glm::quat m_ParentWorldRotation{};

		// Rotation dirty flag
		bool m_RotationDF{ true };

		// -------------------
		// Scale
		// -------------------

		// Scale relative to parent
		glm::vec3 m_LocalScale{1, 1, 1};

		// World scale of parent
		glm::vec3 m_ParentWorldScale{};

		// Scale dirty flag
		bool m_ScaleDF{ true };

		/// <summary>
		/// Get parent position
		/// </summary>
		/// <returns>Reference to parent position</returns>
		const glm::vec3& GetParentPosition();

		/// <summary>
		/// Get parent rotation
		/// </summary>
		/// <returns>Reference to parent rotation</returns>
		const glm::quat& GetParentRotation();

		/// <summary>
		/// Get parent scale
		/// </summary>
		/// <returns>Reference to parent rotation</returns>
		const glm::vec3& GetParentScale();
	};
}
#endif // !_TRANSFORM_
