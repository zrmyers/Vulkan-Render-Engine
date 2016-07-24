/*
    *******************************************************************************
	Application: Vulkan Render Engine
	Version: Version: 1.1.0
	File: application.cpp
	Author: Zane R. Myers
	Last Modified: 7/23/2016
	********************************************************************************

	Application Description:
		This application tests the first revision of the Vulkan Render Engine.
		Capabilites to clear presentation surface to desired color are introduced.

		This will be demonstrated by animating the background color in a window.
*/
#include "common.h"
#include <iostream>

using namespace std;

int main()
{
	cout << "Starting Vulkan Render Engine Demo...\n";

	//initialize GLFW for window and event handling
	glfwInit();
	//since we are using Vulkan library from elsewhere, we don't need GLFW to provide one
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Vulkan Render Engine", nullptr, nullptr);
	if (!window)
	{
		cout << "Failed to create GLFW Window!\n";
		glfwTerminate();
		return -1;
	}

	//change focus to window
	glfwMakeContextCurrent(window);


	//main loop.  while the window isn't closing, keep running.
	//  poll inputs to make sure events are handled.
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	cout << "Vulkan Render Engine Demo Complete...\n";

	//takedown...
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}