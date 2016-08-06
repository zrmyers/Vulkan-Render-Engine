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
#include "interpolator.h"

using namespace std;

float step_size;

struct colors
{
	enum {red,green,blue};
};

glm::vec4 red = {1.0f,0.0f,0.0f,1.0f};
glm::vec4 green = { 0.0f,1.0f,0.0f,1.0f };
glm::vec4 blue = { 0.0f,0.0f,1.0f,1.0f };

int a_color = colors::red;
int b_color = colors::blue;

Interpolate_Info a_info;
Interpolate_Info b_info;

std::vector<glm::vec4> color_table = { red,green,blue };
std::vector<int> color_transition_table = { colors::green,colors::blue,colors::red };

int main()
{
	cout << "Starting Vulkan Render Engine Demo...\n";

	//initialize GLFW for window and event handling
	glfwInit();
	//since we are using Vulkan library from elsewhere, we don't need GLFW to provide one
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* windowA = glfwCreateWindow(1024, 768, "Vulkan Render Engine", nullptr, nullptr);
	if (!windowA)
	{
		cout << "Failed to create GLFW Window A!\n";
		glfwTerminate();
		return -1;
	}

	GLFWwindow* windowB = glfwCreateWindow(1024, 768, "Vulkan Render Engine", nullptr, nullptr);
	if (!windowB)
	{
		cout << "Failed to create GLFW Window B!\n";
		glfwTerminate();
		return -1;
	}
	//change focus to window
	glfwMakeContextCurrent(windowA);
	glfwMakeContextCurrent(windowB);

	RenderEngineInfo re_info;
	re_info.applicationName = "Vulkan Render Engine Test App!\n";
	re_info.app_VMAJOR = 1;
	re_info.app_VMINOR = 0;
	re_info.app_VPATCH = 0;

	//initialize the rendering engine
	RenderEngine* r_engine = new RenderEngine(&re_info);

	r_engine->attachWindow(windowA);
	r_engine->setWindowClearColor(windowA,color_table[a_color]);

	r_engine->attachWindow(windowB);
	r_engine->setWindowClearColor(windowB,color_table[b_color]);	

	//main loop.  while the window isn't closing, keep running.
	//  poll inputs to make sure events are handled.
	bool running = true;
	bool runningA = true;
	bool runningB = true;

	//prepare interpolation stuff
	Interpolator interp;

	a_info.current_state.push_back(color_table[a_color]);
	a_info.desired_state.push_back(color_table[color_transition_table[colors::red]]);
	a_info.duration = 1000.0f;

	b_info.current_state.push_back(color_table[b_color]);
	b_info.desired_state.push_back(color_table[color_transition_table[colors::blue]]);
	b_info.duration = 1000.0f;

	while (running)
	{
		//poll windows
		if (runningA)
		{
			if (glfwWindowShouldClose(windowA))
			{
				r_engine->eventWindowDestroyed(windowA);
				glfwDestroyWindow(windowA);
				runningA = false;
			}
		}

		if (runningB)
		{
			if (glfwWindowShouldClose(windowB))
			{
				r_engine->eventWindowDestroyed(windowB);
				glfwDestroyWindow(windowB);
				runningB = false;
			}
		}

		running = runningA || runningB;

		r_engine->pollWindowResize();

		//update commands
		if (interp.interpolate_linear(&a_info, 0.1f))
		{
			//the desired color has been reached
			a_color = color_transition_table[a_color];

			//need to refresh the interp data
			a_info.current_state[0] = color_table[a_color];
			a_info.desired_state[0] = color_table[color_transition_table[a_color]];
			a_info.duration = 1000.0f;
		}
		r_engine->setWindowClearColor(windowA, a_info.current_state[0]);

		if (interp.interpolate_linear(&b_info, 0.1f))
		{
			//the desired color has been reached
			b_color = color_transition_table[b_color];

			//need to refresh the interp data
			b_info.current_state[0] = color_table[b_color];
			b_info.desired_state[0] = color_table[color_transition_table[b_color]];
			b_info.duration = 1000.0f;
		}
		r_engine->setWindowClearColor(windowB, b_info.current_state[0]);
		//end update commands

		//draw commands

		//end draw commands
		r_engine->recordBuffers();

		r_engine->swapBuffers();

		glfwPollEvents();
	}
	
	cout << "Vulkan Render Engine Demo Complete...\n";

	//delete the rendering engine
	delete r_engine;
	//takedown...
	glfwTerminate();
	return 0;
}

