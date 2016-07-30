#pragma once
#include "common.h"
#include "vk_physicaldevice.h"
#include "vk_device.h"
#include "vk_swapchain.h"

//information needed to create a new instance
typedef struct VK_InstanceInfo
{
	//determined at application level
	const char* pApplicationName;
	uint32_t applicationVersion;
	//determined at engine level
	const char* pEngineName;
	uint32_t engineVersion;
	//determined at renderer level
	uint32_t apiVersion;
	uint32_t enabledLayerCount;
	const char* const* ppEnabledLayerNames;
	uint32_t enabledExtensionCount;
	const char* const* ppEnabledExtensionNames;
	VkAllocationCallbacks* allocs;

}VK_InstanceInfo;

class VK_Instance
{
public:
	VK_Instance(VK_InstanceInfo*);
	~VK_Instance();

	VkInstance* getInstance();
	VK_Swapchain* createSwapchain(VkSurfaceKHR*,uint32_t,uint32_t);

private:
	//private data
	VkInstance instance;
	VkAllocationCallbacks* allocs;	
	VK_PhysicalDevice* physicalDevice;
	VK_Device* device;

	uint32_t apiversion;

	//private methods
	bool checkExtensionAvailability(const char*,std::vector<VkExtensionProperties>);
	void findDevice(VkSurfaceKHR*);
};