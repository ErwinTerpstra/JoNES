#ifndef _WINDOW_H_
#define _WINDOW_H_

struct GLFWwindow;

namespace JoNES
{
	class Window
	{
	private:
		GLFWwindow* handle;

	private:
		Window(GLFWwindow* handle);

	public:
		~Window();

		bool ShouldClose() const;
		void MakeCurrent() const;

		void SetSwapInterval(int interval) const;

		void Present();

		void GetWindowSize(int* width, int* height) const;
		void GetFrameBufferSize(int* width, int* height) const;

		GLFWwindow* GetHandle() { return handle; }

		static Window* Create(int width, int height, const char* title);
	};
}

#endif