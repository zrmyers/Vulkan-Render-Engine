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

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	//data for queue creation with device
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo;
	graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.pNext = nullptr;
	graphicsQueueCreateInfo.flags = 0;
	graphicsQueueCreateInfo.queueFamilyIndex = devinfo->graphicsQueueFamilyIndex; //need to fill in with data from physical device
	graphicsQueueCreateInfo.queueCount = 1;
	graphicsQueueCreateInfo.pQueuePriorities = &queuePriorities;

	queueCreateInfos.push_back(graphicsQueueCreateInfo);

	if (devinfo->graphicsQueueFamilyIndex != devinfo->presentQueueFamilyIndex)
	{
		VkDeviceQueueCreateInfo presentQueueCreateInfo;
		presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentQueueCreateInfo.pNext = nullptr;
		presentQueueCreateInfo.flags = 0;
		presentQueueCreateInfo.queueFamilyIndex = devinfo->presentQueueFamilyIndex;
		presentQueueCreateInfo.queueCount = 1;
		presentQueueCreateInfo.pQueuePriorities = &queuePriorities;

		queueCreateInfos.push_back(presentQueueCreateInfo);
	}

	//data for device creation
	VkDeviceCreateInfo devCreateInfo;
	devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.pNext = nullptr;
	devCreateInfo.flags = 0;
	devCreateInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();
	devCreateInfo.pQueueCreateInfos = &queueCreateInfos[0];
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

	presentQueueFamilyIndex = devinfo->presentQueueFamilyIndex;
	graphicsQueueFamilyIndex = devinfo->graphicsQueueFamilyIndex;

	//let's build some q's
	graphicsQueue = (VkQueue*)malloc(sizeof(VkQueue));
	presentQueue = (VkQueue*)malloc(sizeof(VkQueue));

	vkGetDeviceQueue(*device, graphicsQueueFamilyIndex, 0, graphicsQueue);
	vkGetDeviceQueue(*device, presentQueueFamilyIndex, 0, presentQueue);
}

VkDevice* VK_Renderer::VK_Device::getDevice()
{
	return device;
}


VkQueue* VK_Renderer::VK_Device::getPresentQueue()
{
	return presentQueue;
}


VkQueue* VK_Renderer::VK_Device::getGraphicsQueue()
{
	return graphicsQueue;
}

VK_Renderer::VK_Device::~VK_Device()
{
	if (device != VK_NULL_HANDLE)
	{
		vkQueueWaitIdle(*graphicsQueue);
		free((void*)graphicsQueue);

		vkQueueWaitIdle(*presentQueue);
		free((void*)presentQueue);

		vkDeviceWaitIdle(*device);
		vkDestroyDevice(*device, allocs);
	}
}
