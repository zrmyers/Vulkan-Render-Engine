#include "vk_renderer.h"

VK_Renderer::VK_Surface::VK_Surface(VK_SurfaceInfo* sinfo)
{
	VkResult result;
	surface = (VkSurfaceKHR*)malloc(sizeof(VkSurfaceKHR));
	if ((result = glfwCreateWindowSurface(*(sinfo->instance), sinfo->window, sinfo->allocs, surface)) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Failed to create surface!\n";
		surface = VK_NULL_HANDLE;
	}
	instance = sinfo->instance;
	allocs = sinfo->allocs;
}

VkSurfaceKHR* VK_Renderer::VK_Surface::getSurface()
{
	return surface;
}

VK_Renderer::VK_Surface::~VK_Surface()
{
	vkDestroySurfaceKHR(*instance, *surface, allocs);
	free((void*)surface);
}