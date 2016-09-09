#include "InputManager.h"

void InputManager::MoveMouse(double x, double y)
{
	glfwSetCursorPos(window, x, y);
	mx = x;
	my = y;
}