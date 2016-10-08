#pragma once
#include "common.h"

typedef struct VK_PhysicalDeviceInfo
{
	//determined at renderer level
	VkInstance* instance;
	uint32_t apiversion;
	uint32_t maxDimm2D;
}VK_PhysicalDeviceInfo;

class VK_PhysicalDevice
{
public:
	VK_PhysicalDevice(VK_PhysicalDeviceInfo*);
	~VK_PhysicalDevice();

	VkPhysicalDevice* getPhysicalDevice();
	uint32_t getGraphicsQueueFamilyIndex();
	uint32_t getPresentationQueueFamilyIndex(VkSurfaceKHR*);
private:
	bool checkProperties(VkPhysicalDevice, VK_PhysicalDeviceInfo*);
private:
	VkPhysicalDevice* physicalDevice;
	uint32_t graphics_queue_family_index;
	uint32_t presentation_queue_family_index;
};