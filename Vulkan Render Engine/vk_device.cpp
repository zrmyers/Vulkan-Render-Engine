#include "vk_device.h"

VK_Device::VK_Device(VK_DeviceInfo* devinfo)
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
}

VkDevice* VK_Device::getDevice()
{
	return device;
}

uint32_t VK_Device::getPresentQueueFamilyIndex()
{
	return presentQueueFamilyIndex;
}

uint32_t VK_Device::getGraphicsQueueFamilyIndex()
{
	return graphicsQueueFamilyIndex;
}

void VK_Device::createSemaphore(VkSemaphore* semaphore)
{
	VkSemaphoreCreateInfo semaphinfo;
	semaphinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphinfo.pNext = nullptr;
	semaphinfo.flags = 0;

	if (vkCreateSemaphore(*device, &semaphinfo, allocs, semaphore) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Could not create semaphores!\n";
	}
}

void VK_Device::destroySemaphore(VkSemaphore* semaphore)
{
	vkDestroySemaphore(*device, *semaphore, allocs);
}

void VK_Device::getQueue(VkQueue* queue, uint32_t queueFamilyIndex)
{
	vkGetDeviceQueue(*device, queueFamilyIndex, 0, queue);
}

void VK_Device::createCommandPool(VkCommandPool* commandPool, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo cmd_pool_create_info;
	cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_create_info.pNext = nullptr;
	cmd_pool_create_info.flags = 0;
	cmd_pool_create_info.queueFamilyIndex = queueFamilyIndex;

	if (vkCreateCommandPool(*device, &cmd_pool_create_info, allocs, commandPool) != VK_SUCCESS)
	{
		std::cout << "Could not create command pool!\n";
	}
}

void VK_Device::destroyCommandPool(VkCommandPool* pool)
{
	vkDestroyCommandPool(*device, *pool, allocs);
}

void VK_Device::allocateCommandBuffers(VkCommandPool* pool,VkCommandBuffer* buffers, uint32_t count )
{
	VkCommandBufferAllocateInfo cmdBuffAllocInfo;
	cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBuffAllocInfo.pNext = nullptr;
	cmdBuffAllocInfo.commandPool = *pool;
	cmdBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBuffAllocInfo.commandBufferCount = count;
	
	if (vkAllocateCommandBuffers(*device, &cmdBuffAllocInfo, buffers) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Could not allocate command buffers!\n";
	}
}

VK_Device::~VK_Device()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(*device);
		vkDestroyDevice(*device, allocs);
		free((void*)device);
	}
}
