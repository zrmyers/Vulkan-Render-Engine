#include <iostream>
#include <string>
#include "vk_renderer.h"
#include <GLFW\glfw3.h>

using namespace std;

//global variables
VK_Renderer* renderer;

int main(int argc, char** argv)
{
	//Initialization Segment
	cout << "Initializing Renderer\n";
	VK_RendererInfo rinfo;
	rinfo.applicationName = "Hello Vulkan";
	rinfo.app_VMAJOR = 1;
	rinfo.app_VMINOR = 0;
	rinfo.app_VPATCH = 0;
	rinfo.engineName = "Zeal";
	rinfo.eng_VMAJOR = 1;
	rinfo.eng_VMINOR = 0;
	rinfo.eng_VPATCH = 0;

	if ((renderer = new VK_Renderer(&rinfo)) == nullptr) cout << "Failed to Initialize Renderer!\n";

	//Destruction Segment
	cout << "Destroying Renderer\n";
	if (renderer != nullptr) delete renderer;
}