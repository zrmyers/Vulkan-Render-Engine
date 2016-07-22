#include <iostream>
#include <string>
#include "vk_renderer.h"

using namespace std;

//global variables
VK_Renderer* renderer;

int main(int argc, char** argv)
{	
	//Initialization Segment
	cout << "\nInitializing Window\n";
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Hello Vulkan", nullptr, nullptr);
	if (!window)
	{
		cout << "Failed to Initialize Window!\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	cout << "\nInitializing Renderer\n";
	VK_RendererInfo rinfo;
	rinfo.applicationName = "Hello Vulkan";
	rinfo.app_VMAJOR = 1;
	rinfo.app_VMINOR = 0;
	rinfo.app_VPATCH = 0;
	rinfo.engineName = "Zeality";
	rinfo.eng_VMAJOR = 1;
	rinfo.eng_VMINOR = 0;
	rinfo.eng_VPATCH = 0;
	rinfo.window = window;

	if ((renderer = new VK_Renderer(&rinfo)) == nullptr)
	{
		cout << "Failed to Initialize Renderer!\n";
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{

		//swap image buffers
		if (!renderer->swap())
		{
			cout << "Bad Swap\n";
		}

		glfwPollEvents();
	}
	//Destruction Segment
	cout << "\nDestroying Renderer\n";
	if (renderer != nullptr) delete renderer;

	cout << "\nDestroying Window\n";	
	glfwDestroyWindow(window);
	glfwTerminate();
	cout << "\nDone...\n";
	
	return 0;
}