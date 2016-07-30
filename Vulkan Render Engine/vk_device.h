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
	VkQueue* getPresentQueue();
	VkQueue* getGraphicsQueue();
	VkSemaphore* getImageAvailableSemaphore();
	VkSemaphore* getRenderingFinishedSemaphore();

	void allocateCommandBuffers(uint32_t, VkCommandBuffer*);

private:
	VkDevice* device;
	VkAllocationCallbacks* allocs;
	VkQueue* graphicsQueue;
	VkQueue* presentQueue;
	VkSemaphore* imageAvailableSemaphore;
	VkSemaphore* renderingFinishedSemaphore;
	VkCommandPool* commandPool;

	uint32_t presentQueueFamilyIndex;
	uint32_t graphicsQueueFamilyIndex;
};