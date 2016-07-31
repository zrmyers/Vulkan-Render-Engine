#pragma once
#include "common.h"

typedef struct VK_DeviceInfo
{
	VkPhysicalDevice* physicalDevice;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;
	uint32_t enabledLayerCount;
	const char* const* ppEnabledLayerNames;
	uint32_t enabledExtensionCount;
	const char* const* ppEnabledExtensionNames;
	VkPhysicalDeviceFeatures* pEnabledFeatures;
	VkAllocationCallbacks* allocs;
}VK_DeviceInfo;

class VK_Device
{
public:
	VK_Device(VK_DeviceInfo*);
	~VK_Device();

	VkDevice* getDevice();
	uint32_t getPresentQueueFamilyIndex();
	uint32_t getGraphicsQueueFamilyIndex();

	
	void createCommandPool(VkCommandPool*,uint32_t);
	void createSemaphore(VkSemaphore*);

	void allocateCommandBuffers(VkCommandPool*, VkCommandBuffer*, uint32_t);

	void getQueue(VkQueue*, uint32_t);

	void destroyCommandPool(VkCommandPool*);
	void destroySemaphore(VkSemaphore*);

private:
	VkDevice* device;
	VkAllocationCallbacks* allocs;

	uint32_t presentQueueFamilyIndex;
	uint32_t graphicsQueueFamilyIndex;
};