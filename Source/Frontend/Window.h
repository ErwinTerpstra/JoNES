#ifndef _WINDOW_H_
#define _WINDOW_H_

struct GLFWwindow;

namespace jones
{
	class Window
	{
	private:
		GLFWwindow* handle;

	private:
		Window(GLFWwindow* handle);
		~Window();

	public:
		bool ShouldClose() const;
		void MakeCurrent() const;

		void Present();

		static Window* Create(int width, int height, const char* title);
	};
}

#endif