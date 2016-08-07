#pragma once
#include "common.h"

typedef struct VK_SwapchainInfo
{
	uint32_t screen_width;
	uint32_t screen_height;
	VkPhysicalDevice* physicalDevice;
	VkSurfaceKHR* surface;
	VkDevice* device;
	VkAllocationCallbacks* allocs;
}VK_SwapchainInfo;

class VK_Swapchain
{
public:
	VK_Swapchain(VK_SwapchainInfo*);
	~VK_Swapchain();

	VkSwapchainKHR* getSwapchain();
	uint32_t getImageCount();
	VkFormat getFormat();
	std::vector<VkImage>* getImages();

private:
	VkSwapchainKHR* swapchain;
	VkAllocationCallbacks* allocs;
	VkDevice* device;
	VkFormat format;

	std::vector<VkImage> images;
};