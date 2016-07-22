#include "vk_renderer.h"

VK_Renderer::VK_Renderer(VK_RendererInfo* rinfo)
{	
	//extension needed for instance
	uint32_t i_ext_count;
	const char** i_extensions = glfwGetRequiredInstanceExtensions(&i_ext_count);
	
	std::vector<const char*> d_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	//information to create instance object
	VK_InstanceInfo instInfo;	
	instInfo.pApplicationName = rinfo->applicationName.c_str();
	instInfo.applicationVersion = VK_MAKE_VERSION(rinfo->app_VMAJOR, rinfo->app_VMINOR, rinfo->app_VPATCH);
	instInfo.pEngineName = rinfo->engineName.c_str();
	instInfo.enabledExtensionCount = i_ext_count;	
	instInfo.ppEnabledExtensionNames = &i_extensions[0];
	instInfo.enabledLayerCount = 0;	 
	instInfo.ppEnabledLayerNames = nullptr;
	instInfo.allocs = nullptr;
	instInfo.apiVersion = 0; //current version of vulkan

	//create instance object
	std::cout << "Creating Vulkan Instance!\n";
	if ((instance = new VK_Instance(&instInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create vulkan instance\n";
		return;
	}

	//information to create physical device object
	VK_PhysicalDeviceInfo physInfo;
	physInfo.instance = instance->getInstance();
	physInfo.minAPIVersionMajor = 1;
	physInfo.minAPIVersionMinor = 0;
	physInfo.minAPIVersionPatch = 0;
	physInfo.maxDimm2D = 4096;

	//create physical device object
	std::cout << "Creating Vulkan Physical Device Instance\n";
	if ((physicalDevice = new VK_PhysicalDevice(&physInfo)) == nullptr)
	{
		std::cout << "Error: Failed to obtain physical device with vulkan support!\n";
		return;
	}

	//information to create a window surface
	VK_SurfaceInfo sinfo;
	sinfo.instance = instance->getInstance();
	sinfo.allocs = nullptr;
	sinfo.window = rinfo->window;

	//create a window surface
	std::cout << "Creating Vulkan Surface Instance\n";
	if ((surface = new VK_Surface(&sinfo)) == nullptr)
	{
		std::cout << "Error: Failed to create a surface!\n";
		return;
	}

	//information to create logical device object
	VK_DeviceInfo devInfo;
	devInfo.physicalDevice = physicalDevice->getPhysicalDevice();
	devInfo.graphicsQueueFamilyIndex = physicalDevice->getGraphicsQueueFamilyIndex();
	devInfo.presentQueueFamilyIndex = physicalDevice->getPresentationQueueFamilyIndex(surface->getSurface());
	devInfo.enabledExtensionCount = (uint32_t) d_extensions.size();
	devInfo.ppEnabledExtensionNames = &d_extensions[0];
	devInfo.enabledLayerCount = 0;
	devInfo.ppEnabledLayerNames = nullptr;
	devInfo.pEnabledFeatures = nullptr;
	devInfo.allocs = nullptr;

	presentQueueFamilyIndex = devInfo.presentQueueFamilyIndex;
	graphicsQueueFamilyIndex = devInfo.graphicsQueueFamilyIndex;

	//create logical device object
	std::cout << "Creating Vulkan Device Instance\n";
	if ((device = new VK_Device(&devInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create logical device!\n";
		return;
	}

	//information to create swap chain object
	VK_SwapchainInfo swapInfo;
	swapInfo.physicalDevice = physicalDevice->getPhysicalDevice();
	swapInfo.surface = surface->getSurface();
	swapInfo.screen_width = rinfo->screen_width;
	swapInfo.screen_height = rinfo->screen_height;
	swapInfo.device = device->getDevice();
	swapInfo.allocs = nullptr;

	//create swap chain object
	std::cout << "Creating Vulkan Swapchain Instance\n";
	if ((swapchain = new VK_Swapchain(&swapInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create swapchain!\n";
		return;
	}

	//need a command buffer for each image
	uint32_t swapchain_image_count = swapchain->getSwapchainImageCount();
	if (swapchain_image_count <= 0)
	{
		std::cout << "Error: Not enough images for swapchain";

	}
	
	presentQueueCmdBuffers.resize(swapchain_image_count);

	device->allocateCommandBuffers(swapchain_image_count, &presentQueueCmdBuffers[0]);

}

bool VK_Renderer::recordBuffers()
{
	uint32_t image_count = (uint32_t) presentQueueCmdBuffers.size();

	std::vector<VkImage> swapchainImages(image_count);

	if (vkGetSwapchainImagesKHR(*device->getDevice(), *swapchain->getSwapchain(), &image_count, &swapchainImages[0]) != VK_SUCCESS)
	{
		std::cout << "Could not get swapchain images!\n";
		return false;
	}

	VkCommandBufferBeginInfo cmdBufferBeginInfo;
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.pNext = nullptr;
	cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	cmdBufferBeginInfo.pInheritanceInfo = nullptr;

	VkClearColorValue clear_color = {
		{1.0f, 0.8f, 0.4f, 0.0f}
	};

	VkImageSubresourceRange isr;
	isr.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	isr.baseMipLevel = 0;
	isr.levelCount = 1;
	isr.baseArrayLayer = 0;
	isr.layerCount = 1;

	for (uint32_t i = 0; i < image_count; i++)
	{
		VkImageMemoryBarrier ptc;
		ptc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		ptc.pNext = nullptr;
		ptc.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		ptc.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		ptc.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ptc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		ptc.srcQueueFamilyIndex = presentQueueFamilyIndex;
		ptc.dstQueueFamilyIndex = presentQueueFamilyIndex;
		ptc.image = swapchainImages[i];
		ptc.subresourceRange = isr;
		
		VkImageMemoryBarrier ctp;
		ctp.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		ctp.pNext = nullptr;
		ctp.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		ctp.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		ctp.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		ctp.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		ctp.srcQueueFamilyIndex = presentQueueFamilyIndex;
		ctp.dstQueueFamilyIndex = presentQueueFamilyIndex;
		ctp.image = swapchainImages[i];
		ctp.subresourceRange = isr;

		vkBeginCommandBuffer(presentQueueCmdBuffers[i], &cmdBufferBeginInfo);

		vkCmdPipelineBarrier(
			presentQueueCmdBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &ptc
		);

		vkCmdClearColorImage(
			presentQueueCmdBuffers[i],
			swapchainImages[i],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			&clear_color,
			1,
			&isr
		);

		vkCmdPipelineBarrier(
			presentQueueCmdBuffers[i],
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &ctp
		);

		if (vkEndCommandBuffer(presentQueueCmdBuffers[i]) != VK_SUCCESS)
		{
			std::cout << "Could not record command buffers!\n";
		}
	}
}

bool VK_Renderer::swapBuffers()
{
	//obtain an image which we would like to present
	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(*device->getDevice(), *swapchain->getSwapchain(), 
											UINT64_MAX, *device->getImageAvailableSemaphore(), 
											VK_NULL_HANDLE, &image_index);
	switch (result)
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		std::cout << "Swapchain is out of date!\n";
		return false;
	default:
		std::cout << "Problem occurred during swap chain image acquisition!";
		return false;
	}

	//present queue to swapchain for rendering
	if(!SubmitQueue(image_index)) return false;

	//present queue to swapchain for presentation
	if(!PresentQueue(&image_index)) return false;

	return true;
}

bool VK_Renderer::SubmitQueue(uint32_t image_index)
{
	//submit a queue to the swap chain for rendering
	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

	VkSubmitInfo subinfo;
	subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	subinfo.pNext = nullptr;
	subinfo.waitSemaphoreCount = 1;
	subinfo.pWaitSemaphores = device->getImageAvailableSemaphore();
	subinfo.pWaitDstStageMask = &wait_dst_stage_mask;
	subinfo.commandBufferCount = 1;
	subinfo.pCommandBuffers = &presentQueueCmdBuffers[image_index];
	subinfo.signalSemaphoreCount = 1;
	subinfo.pSignalSemaphores = device->getRenderingFinishedSemaphore();

	if (vkQueueSubmit(*device->getPresentQueue(), 1, &subinfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

bool VK_Renderer::PresentQueue(uint32_t* image_index)
{
	VkResult result;
	//submit a queue to the swap chain for presentation
	VkPresentInfoKHR presinfo;
	presinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presinfo.pNext = nullptr;
	presinfo.waitSemaphoreCount = 1;
	presinfo.pWaitSemaphores = device->getRenderingFinishedSemaphore();
	presinfo.swapchainCount = 1;
	presinfo.pSwapchains = swapchain->getSwapchain();
	presinfo.pImageIndices = image_index;
	presinfo.pResults = nullptr;

	result = vkQueuePresentKHR(*device->getPresentQueue(), &presinfo);

	switch (result)
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
	case VK_ERROR_OUT_OF_DATE_KHR:
		return false;
	default:
		std::cout << "Problem occurred during swapchain image presentation!\n";
		return false;
	}

	return true;
}

VK_Renderer::~VK_Renderer()
{
	std::cout << "Destroying Vulkan Swapchain Instance\n";
	if (swapchain != nullptr) delete swapchain;
	std::cout << "Destroying Vulkan Device Instance\n";
	if (device != nullptr) delete device;
	std::cout << "Destroying Vulkan Surface Instance\n";
	if (device != nullptr) delete surface;
	std::cout << "Destroying Vulkan Physical Device Instance\n";
	if (physicalDevice != nullptr) delete physicalDevice;
	std::cout << "Destroying Vulkan Instance\n";
	if (instance != nullptr) delete instance;
	
}