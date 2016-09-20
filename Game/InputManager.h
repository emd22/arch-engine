#pragma once
#include <array>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class InputManager
{
public:
	std::map<int, bool> keys;

	double mx, my;

	void MoveMouse(double x, double y);

	GLFWwindow *window;
};

