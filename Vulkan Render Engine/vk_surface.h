#pragma once
#include "common.h"

typedef struct VK_SurfaceInfo
{
	VkInstance* instance;
	GLFWwindow* window;
	VkAllocationCallbacks* allocs;
}VK_SurfaceInfo;

class VK_Surface
{
public:
	VK_Surface(VK_SurfaceInfo*);
	~VK_Surface();

	VkSurfaceKHR* getSurface();
private:
	VkSurfaceKHR* surface;
	VkAllocationCallbacks* allocs;
	VkInstance* instance;
};