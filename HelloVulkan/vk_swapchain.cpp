#include "vk_renderer.h"

VK_Renderer::VK_Swapchain::VK_Swapchain(VK_SwapchainInfo* swapinfo)
{
	swapchain = (VkSwapchainKHR*)malloc(sizeof(VkSwapchainKHR));
	allocs = swapinfo->allocs;

	//Acquire Surface Capabilities
	VkSurfaceCapabilitiesKHR surface_capabilities;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*(swapinfo->physicalDevice), *(swapinfo->surface), &surface_capabilities) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to obtain surface capabilities!\n";
	}

	//Acquire Accepted Image Formats
	uint32_t formats_count;
	if (((vkGetPhysicalDeviceSurfaceFormatsKHR(*(swapinfo->physicalDevice), *(swapinfo->surface), &formats_count, nullptr)) != VK_SUCCESS) || (formats_count == 0))
	{
		std::cout << "VK Error: Failed to enumerate surface formats!\n";
		return;
	}

	std::vector<VkSurfaceFormatKHR> surface_formats(formats_count);
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(*(swapinfo->physicalDevice), *(swapinfo->surface), &formats_count, &surface_formats[0]) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to enumerate surface formats!\n";
		return;
	}

	//Acquire Supported Present Modes
	uint32_t present_modes_count;
	if ((vkGetPhysicalDeviceSurfacePresentModesKHR(*(swapinfo->physicalDevice), *(swapinfo->surface), &present_modes_count, nullptr) != VK_SUCCESS) || (present_modes_count == 0))
	{
		std::cout << "VK Error: Failed to enumerate surface present modes!\n";
		return;
	}

	std::vector<VkPresentModeKHR> present_modes(present_modes_count);
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(*(swapinfo->physicalDevice), *(swapinfo->surface), &present_modes_count, &present_modes[0]) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to enumerate surface present modes!\n";
		return;
	}

	//Get the required number of images for effective swapchain operation
	uint32_t image_count = surface_capabilities.minImageCount + 1;
	if ((surface_capabilities.maxImageCount > 0) && (image_count > surface_capabilities.maxImageCount))
	{
		image_count = surface_capabilities.maxImageCount;
	}

	VkSurfaceFormatKHR surface_format;
	//Get the required swapchain image format
	if ((surface_formats.size() == 1) && (surface_formats[0].format == VK_FORMAT_UNDEFINED))
	{
		surface_format = { VK_FORMAT_R8G8B8A8_UNORM,VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}
	else
	{
		surface_format = surface_formats[0];

		for (VkSurfaceFormatKHR &sf_index : surface_formats)
		{
			if (sf_index.format == VK_FORMAT_R8G8B8A8_UNORM)
			{
				surface_format = sf_index;
				break;
			}
		}
	}

	//Set the swapchain extent
	VkExtent2D swap_chain_extent;
	if (surface_capabilities.currentExtent.width == -1)
	{
		swap_chain_extent = { swapinfo->screen_height,swapinfo->screen_height };
		if (swap_chain_extent.width < surface_capabilities.minImageExtent.width)
		{
			swap_chain_extent.width = surface_capabilities.minImageExtent.width;
		}
		if (swap_chain_extent.height < surface_capabilities.minImageExtent.height)
		{
			swap_chain_extent.height = surface_capabilities.minImageExtent.height;
		}
		if (swap_chain_extent.width > surface_capabilities.maxImageExtent.width)
		{
			swap_chain_extent.width = surface_capabilities.maxImageExtent.width;
		}
		if (swap_chain_extent.height > surface_capabilities.maxImageExtent.height)
		{
			swap_chain_extent.height = surface_capabilities.maxImageExtent.height;
		}
	}
	else
	{
		swap_chain_extent = surface_capabilities.currentExtent;
	}

	//Set usage flags
	VkImageUsageFlags image_usage_flags;
	if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		image_usage_flags = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	}
	else
	{
		std::cout << "VK_IMAGE_USAGE_TRANSFER_DST image is not supported by the swapchain!\n"
			<< "Supported swap chain image usage flags include: \n"
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT ? "   VK_IMAGE_USAGE_TRANSFER_SRC\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ? "   VK_IMAGE_USAGE_TRANSFER_DST\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? "   VK_IMAGE_USAGE_SAMPLED_BIT\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT ? "   VK_IMAGE_USAGE_STORAGE_BIT\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ? "   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? "   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n" : "")
			<< (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT ? "   VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n" : "");
		image_usage_flags = -1;
	}

	//set transform flags
	VkSurfaceTransformFlagBitsKHR transform_flags;
	if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		transform_flags = surface_capabilities.currentTransform;
	}

	//set swap chain present mode
	VkPresentModeKHR present_mode = (VkPresentModeKHR) -1;
	for (VkPresentModeKHR &pm_index : present_modes)
	{
		if (pm_index == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			present_mode = pm_index;
			break;
		}
		else if (pm_index == VK_PRESENT_MODE_FIFO_KHR)
		{
			present_mode = pm_index;
		}
	}

	if (present_mode == (VkPresentModeKHR)-1)
	{
		std::cout << "Vulkan Error: FIFO present mode not supported by the swap chain!\n";
	}

	if ((image_usage_flags == 1) || (present_mode == (VkPresentModeKHR)-1))
	{
		return;
	}

	VkSwapchainCreateInfoKHR swinfo;
	swinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swinfo.pNext = nullptr;
	swinfo.flags = 0;
	swinfo.surface = *(swapinfo->surface);
	swinfo.minImageCount = image_count;
	swinfo.imageFormat = surface_format.format;
	swinfo.imageColorSpace = surface_format.colorSpace;
	swinfo.imageExtent = swap_chain_extent;
	swinfo.imageArrayLayers = 1;
	swinfo.imageUsage = image_usage_flags;
	swinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swinfo.queueFamilyIndexCount = 0;
	swinfo.pQueueFamilyIndices = nullptr;
	swinfo.preTransform = transform_flags;
	swinfo.presentMode = present_mode;
	swinfo.clipped = VK_TRUE;
	swinfo.oldSwapchain = VK_NULL_HANDLE;
	swinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	if (vkCreateSwapchainKHR(*(swapinfo->device), &swinfo, allocs, swapchain) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Could not create swap chain!\n";
		return;
	}

	device = swapinfo->device;
}

VK_Renderer::VK_Swapchain::~VK_Swapchain()
{
	vkDestroySwapchainKHR(*device,*swapchain,allocs);
	free((void*)swapchain);
}