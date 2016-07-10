#include "vk_renderer.h"

VK_Renderer::VK_Device::VK_Device(VK_DeviceInfo* devinfo)
{
	if ((devinfo->physicalDevice) == VK_NULL_HANDLE)
	{
		std::cout << "Vulkan Error: Failed to create appropriate vulkan device!\n";
		device = VK_NULL_HANDLE;
		return;
	}

	device = (VkDevice*)malloc(sizeof(VkDevice));
	float queuePriorities = 1.0f; //this may be renderer level information

								  //data for queue creation with device
	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = devinfo->graphicsQueueFamilyIndex; //need to fill in with data from physical device
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	//data for device creation
	VkDeviceCreateInfo devCreateInfo;
	devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.pNext = nullptr;
	devCreateInfo.flags = 0;
	devCreateInfo.queueCreateInfoCount = 1;
	devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	devCreateInfo.enabledLayerCount = devinfo->enabledLayerCount;
	devCreateInfo.ppEnabledLayerNames = devinfo->ppEnabledLayerNames;
	devCreateInfo.enabledExtensionCount = devinfo->enabledExtensionCount;
	devCreateInfo.ppEnabledExtensionNames = devinfo->ppEnabledExtensionNames;
	devCreateInfo.pEnabledFeatures = devinfo->pEnabledFeatures;

	allocs = devinfo->allocs;

	if (vkCreateDevice(*(devinfo->physicalDevice), &devCreateInfo, allocs, device) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Could Not create Vulkan device!\n";
		device = VK_NULL_HANDLE;
	}
}

VkDevice* VK_Renderer::VK_Device::getDevice()
{
	return device;
}

VK_Renderer::VK_Device::~VK_Device()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(*device);
		vkDestroyDevice(*device, allocs);
	}
}
