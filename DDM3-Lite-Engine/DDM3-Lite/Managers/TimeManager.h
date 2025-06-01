// TimeManager.h
// This singleton will store and manage all information about time

#ifndef TimeManagerIncluded
#define TimeManagerIncluded

// File includes
#include "Engine/Singleton.h"

namespace DDM3
{
	class TimeManager final : public Singleton<TimeManager>
	{
	public:
		// Set the delta time of this frame
		// Parameters:
		//     dt: the current value of delta time
		void SetDeltaTime(float dt);

		// Get the current delta time
		float GetDeltaTime() const { return m_DeltaTime; }

		float GetDeltaTimeMS() const { return m_DeltaTime * 1000; }

		// Get the current amount of frames per second
		int GetFps() const { return m_Fps; }

		// Set the fixed time step
		// Parameters:
		//     ft: teh value of the fixed time step
		void SetFixedTime(float ft) { m_FixedTime = ft; }

		// Get the value of the fixed time step
		float GetFixedTime() const { return m_FixedTime; }

	private:
		friend class Singleton<TimeManager>;
		TimeManager() = default;

		// Frames per second of current frame
		int m_Fps = 0;

		// Delta time of current frame
		float m_DeltaTime = 0;

		// Fixed time step
		float m_FixedTime = 0.2f;
	};
}


#endif // !TimeManagerIncluded