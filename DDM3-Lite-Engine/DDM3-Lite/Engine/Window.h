// Window.h
// This class will hold the singleton for the GLFW window

#ifndef WindowIncluded
#define WindowIncluded

// File includes
#include "Singleton.h"
#include "Includes/GLFWIncludes.h"

// Standard library includes
#include <functional>
#include <map>

namespace DDM
{
	// Window struct
	// Member variables:
	//     -PWindow: pointer to the glfwWindow
	//     -width: windowWidth
	//     -height: windowHeight
	//     -frameBufferResized: will be set to true when window is resized
	struct WindowStruct
	{
		GLFWwindow* pWindow{};
		int Width{};
		int Height{};
		bool FrameBufferResized = false;
	};

	class Window : public Singleton<Window>
	{
	public:
		// Constructor
		Window();

		// Default
		~Window();

		// Get a reference to the windowstruct object
		WindowStruct& GetWindowStruct() { return m_Window; }

		// Set the FrameBufferResized variable
		void SetFrameBufferResized(bool value);

		void AddCallback(void* object, std::function<void(int, const char**)> function);

		void RemoveCallback(void* object);

	private:
		// List of function callbacks
		std::map<void*, std::function<void(int, const char**)>> m_DropFileCallbacks{};

		// The window struct object
		WindowStruct m_Window{};

		// Function that will initialize the glfw window
		void InitWindow();

		// Function that is called when window is resized
		// Parameters:
		//     pWindow: pointer to the window
		//     width: the new width of the window
		//     height: the new height of the window
		static void FramebufferResizeCallback(GLFWwindow* pWindow, int width, int height);

		// Function that is called when window is maximized or minimized
		// Parameters:
		//     pWindow: pointer to the window
		//     maximized: int that indicates if window was maximized or restored
		static void MaximizeWindowCallback(GLFWwindow* pWindow, int maximized);

		// Function that is called when file is dropped on this window
		// Parameters:
		//     pWindow: pointer to the window
		//     count: amount of files dropped
		//     paths: pointer to file path list
		static void DropFileCallback(GLFWwindow* pWindow, int count, const char** paths);

		void CallDropFileCallbacks(int count, const char** paths);
	};
}

#endif // !WindowIncluded