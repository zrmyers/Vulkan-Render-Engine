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

	bool pollResize();

	uint32_t getWidth();
	uint32_t getHeight();
private:
	VkSurfaceKHR* surface;
	VkAllocationCallbacks* allocs;
	VkInstance* instance;
	GLFWwindow* window;

	int width, height;

	void resize();
};