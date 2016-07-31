/*
    *******************************************************************************
	Application: Vulkan Render Engine
	Version: Version: 1.0.0
	File: application.cpp
	Author: Zane R. Myers
	Last Modified: 7/23/2016
	********************************************************************************

	Application Description:
		This application tests the first revision of the Vulkan Render Engine.
		Capabilites introduced:
			clear specified presentation surface to desired color
			present to multiple windows
			present to multiple surfaces

		This will be demonstrated by animating the background color in a window.
*/
#include "common.h"
#include "render_engine.h"

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

	RenderEngineInfo re_info;
	re_info.applicationName = "Vulkan Render Engine Test App!\n";
	re_info.app_VMAJOR = 1;
	re_info.app_VMINOR = 0;
	re_info.app_VPATCH = 0;

	//initialize the rendering engine
	RenderEngine* r_engine = new RenderEngine(&re_info);

	r_engine->attachWindow(window);

	r_engine->recordBuffers();

	//main loop.  while the window isn't closing, keep running.
	//  poll inputs to make sure events are handled.
	while (!glfwWindowShouldClose(window))
	{
		
		r_engine->swapBuffers();

		glfwPollEvents();
	}
	
	cout << "Vulkan Render Engine Demo Complete...\n";

	//delete the rendering engine
	delete r_engine;
	//takedown...
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}