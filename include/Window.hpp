#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <stdexcept>

namespace re {

	class Window {
	public:
		Window(std::string const& title, int width, int height);
		inline bool open(void) { return !glfwWindowShouldClose(ptr); }
		inline void pollEvents(void) { glfwPollEvents(); }
		~Window(void);

		GLFWwindow* ptr;

	private:
	};

}