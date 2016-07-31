#include "vk_surface.h"

VK_Surface::VK_Surface(VK_SurfaceInfo* sinfo)
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
	window = sinfo->window;

	glfwGetWindowSize(window, &width, &height);
}

VkSurfaceKHR* VK_Surface::getSurface()
{
	return surface;
}

uint32_t VK_Surface::getWidth()
{
	return width;
}

uint32_t VK_Surface::getHeight()
{
	return height;
}

bool VK_Surface::pollResize()
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);

	if (width != w || height != h)
	{
		resize();
		return true;
	}

	return false;
}

void VK_Surface::resize()
{
	VkResult result;
	vkDestroySurfaceKHR(*instance, *surface, allocs);
	
	if ((result = glfwCreateWindowSurface(*instance, window, allocs, surface)) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Failed to create surface!\n";
		surface = VK_NULL_HANDLE;
	}

	glfwGetWindowSize(window, &width, &height);
}

VK_Surface::~VK_Surface()
{
	vkDestroySurfaceKHR(*instance, *surface, allocs);
	free((void*)surface);
}