#include "vk_renderer.h"

VK_Renderer::VK_PhysicalDevice::VK_PhysicalDevice(VK_PhysicalDeviceInfo* physInfo)
{
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice* physicalDevices;

	physicalDevice = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice));

	if ((result = vkEnumeratePhysicalDevices(*(physInfo->instance), &physicalDeviceCount, nullptr)) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to find physical devices!\n";
		physicalDevice = VK_NULL_HANDLE;
		return;
	}

	if (physicalDeviceCount < 1)
	{
		std::cout << "VK Error: No vulkan compatible devices exist!\n";
		physicalDevice = VK_NULL_HANDLE;
		return;
	}
	else
	{
		physicalDevices = (VkPhysicalDevice*)malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));

		if ((result = vkEnumeratePhysicalDevices(*(physInfo->instance), &physicalDeviceCount, physicalDevices)) != VK_SUCCESS)
		{
			std::cout << "VK Error: No vulkan compatible devices found!\n";
			physicalDevice = VK_NULL_HANDLE;
			return;
		}
		//pick the device with correct specs
		for (uint32_t i = 0; i < physicalDeviceCount; i++)
		{
			if (checkProperties(physicalDevices[i], physInfo))
			{
				*physicalDevice = physicalDevices[i];
				break;
			}
		}

		free((void*)physicalDevices);
	}
}

bool VK_Renderer::VK_PhysicalDevice::checkProperties(VkPhysicalDevice phydev, VK_PhysicalDeviceInfo* info)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(phydev, &properties);
	vkGetPhysicalDeviceFeatures(phydev, &features);

	//is the proper vulkan api version supported?
	uint32_t api_major = VK_VERSION_MAJOR(properties.apiVersion);
	uint32_t api_minor = VK_VERSION_MINOR(properties.apiVersion);
	uint32_t api_patch = VK_VERSION_PATCH(properties.apiVersion);

	//make sure maximal screen dimmensions are supported as well as minimal api is supported
	if ((api_major < info->minAPIVersionMajor) &&
		(api_minor < info->minAPIVersionMinor) &&
		(api_patch < info->minAPIVersionPatch) &&
		(properties.limits.maxImageDimension2D < info->maxDimm2D))
	{
		std::cout << "Physical device " << phydev << " does not support required parameters!" << std::endl;
		return false;
	}

	//make sure device has a graphics queue family
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(phydev, &queue_family_count, nullptr);

	if (queue_family_count == 0)
	{
		std::cout << "Physical device " << phydev << " has no queue families!\n";
		return false;
	}

	std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(phydev, &queue_family_count, &queue_family_properties[0]);

	graphics_queue_family_index = UINT32_MAX;
	for (uint32_t i = 0; i < queue_family_count; i++)
	{
		if ((queue_family_properties[i].queueCount > 0) &&
			(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			graphics_queue_family_index = i;
			presentation_queue_family_index = i;  // we will verify this once we fetch the presentation queue family index
			return true;
		}
	}

	return false;
}

uint32_t VK_Renderer::VK_PhysicalDevice::getGraphicsQueueFamilyIndex()
{
	return graphics_queue_family_index;
}

uint32_t VK_Renderer::VK_PhysicalDevice::getPresentationQueueFamilyIndex(VkSurfaceKHR* surface)
{
	VkBool32 result;
	vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, presentation_queue_family_index, *surface, &result);

	if (!result)
	{
		//need to find a family that does support presentation
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queue_family_count, nullptr);

		if (queue_family_count == 0)
		{
			std::cout << "Physical device " << physicalDevice << " has no queue families!\n";
			return presentation_queue_family_index;
		}

		std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queue_family_count, &queue_family_properties[0]);

		//for each family, check that it supports a presentation queue
		for (uint32_t i = 0; i < queue_family_count; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &result);

			if (result)
			{
				presentation_queue_family_index = i;
				break;
			}
		}
	}

	return presentation_queue_family_index;
}

VkPhysicalDevice* VK_Renderer::VK_PhysicalDevice::getPhysicalDevice()
{
	return physicalDevice;
}

VK_Renderer::VK_PhysicalDevice::~VK_PhysicalDevice()
{
	free((void*)physicalDevice);
}