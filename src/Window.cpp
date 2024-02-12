#pragma once

#include "Window.hpp"

re::Window::Window(std::string const& title, int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	ptr = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!ptr)
		throw std::runtime_error("failed to create window");

}

re::Window::~Window(void)
{
	glfwDestroyWindow(ptr);
	glfwTerminate();
}