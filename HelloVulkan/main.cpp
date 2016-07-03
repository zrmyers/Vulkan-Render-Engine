#include <iostream>
#include <GLFW\glfw3.h>
#include <stdlib.h>
#include <vulkan\vulkan.h>

using namespace std;

int main(int argc, char** argv)
{
	//create a vulkan instance
	VkInstance* instance;
	VkInstanceCreateInfo vkici;
	VkResult vr;

	memset(&vkici, 0, sizeof(VkInstanceCreateInfo));
	vr = vkCreateInstance(&vkici, NULL, instance);
	if (vr == VK_SUCCESS)
	{
		cout << "Created Instance Sucessfully!\n";
	}

	//destroy vulkan instance
	vkDestroyInstance(*instance, NULL);
}