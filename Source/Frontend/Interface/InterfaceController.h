#ifndef _INTERFACE_CONTROLLER_H_
#define _INTERFACE_CONTROLLER_H_

struct GLFWwindow;
struct GLFWcursor;

namespace JoNES
{
	class Window;

	class InterfaceController
	{
	private:
		Window& window;

		bool mouseDown[5];
		GLFWcursor* mouseCursors[8];

	public:
		InterfaceController(Window& window);
		~InterfaceController();

		void Update(float dt);

	private:
		void UpdateMouse();
		void UpdateCursor();

		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void CharCallback(GLFWwindow* window, unsigned int c);
	};
}

#endif