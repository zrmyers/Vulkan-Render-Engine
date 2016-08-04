#include "render_engine.h"
#include "vk_instance.h"
#include "vk_surface.h"
#include "vk_swapchain.h"
#include "vk_device.h"

#include <stdlib.h>
#include <vector>

RenderEngine::RenderEngine(RenderEngineInfo* re_info)
{
	uint32_t requiredExtensionCount;
	const char** requiredExtensions;

	instance = nullptr;

	//put together required information for obtaining
	VK_InstanceInfo instanceInfo;
	instanceInfo.pApplicationName = re_info->applicationName.c_str();
	std::cout << re_info->applicationName << "\n";
	instanceInfo.applicationVersion = VK_MAKE_VERSION(re_info->app_VMAJOR,re_info->app_VMINOR,re_info->app_VPATCH);
	instanceInfo.pEngineName = "Vulkan Render Engine\n";
	instanceInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
	instanceInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	
	//need instance level extensions for GLFW surface use
	requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	instanceInfo.ppEnabledExtensionNames = requiredExtensions;
	instanceInfo.enabledExtensionCount = requiredExtensionCount;

	//specify layers used.  Right now that is none
	instanceInfo.ppEnabledLayerNames = nullptr;
	instanceInfo.enabledLayerCount = 0;

	//we don't care about using our own allocation callbacks right now. set to null
	instanceInfo.allocs = nullptr;

	instance = new VK_Instance(&instanceInfo);

}

void RenderEngine::attachWindow(GLFWwindow* window)
{
	int width;
	int height;

	RenderContext context;
	//add window to context	
	context.window = window;

	//create a surface for that window.  
	//A window can have multiple surfaces, which is handled by attaching a window twice.
	VK_SurfaceInfo sinfo;
	sinfo.instance = instance->getInstance();
	sinfo.window = window;
	sinfo.allocs = nullptr;

	VK_Surface* surface = new VK_Surface(&sinfo);

	context.surface = surface;
	//we now have enough information to create a swapchain

	//for every surface, there is exactly one swapchain
	glfwGetWindowSize(window, &width, &height);

	VK_Swapchain* swapchain = instance->createSwapchain(surface->getSurface(),(uint32_t) width,(uint32_t) height);

	context.swapchain = swapchain;

	if (device == nullptr)
	{
		device = instance->getDevice();

		presentQueueFamilyIndex = device->getPresentQueueFamilyIndex();
		graphicsQueueFamilyIndex = device->getGraphicsQueueFamilyIndex();

		presentQueue = (VkQueue*)malloc(sizeof(VkQueue));
		graphicsQueue = (VkQueue*)malloc(sizeof(VkQueue));
		commandPool = (VkCommandPool*)malloc(sizeof(VkCommandPool));
		imageAvailable = (VkSemaphore*)malloc(sizeof(VkSemaphore));
		renderingFinished = (VkSemaphore*)malloc(sizeof(VkSemaphore));

		device->getQueue(presentQueue, presentQueueFamilyIndex);
		device->getQueue(graphicsQueue, graphicsQueueFamilyIndex);
		device->createCommandPool(commandPool, presentQueueFamilyIndex);
		
		device->createSemaphore(imageAvailable);
		device->createSemaphore(renderingFinished);
	}	

	
	context.imageCount = swapchain->getImageCount();
	context.buffers.resize(context.imageCount);

	device->allocateCommandBuffers(commandPool, &context.buffers[0], context.imageCount);

	context.clear_color.float32[0] = 0.2f;
	context.clear_color.float32[1] = 0.2f;
	context.clear_color.float32[2] = 0.2f;
	context.clear_color.float32[3] = 0.2f;

	contexts.push_back(context);
}

void RenderEngine::pollWindowResize()
{
	for( size_t i = 0; i < contexts.size(); i++)
	{
		RenderContext context = contexts.at(i);

		//if the window size has changed the surface and swapchain need to be rebuilt
		if (context.surface->pollResize())
		{			
			vkQueueWaitIdle(*presentQueue);
			vkQueueWaitIdle(*graphicsQueue);

			delete context.swapchain;

			context.swapchain = instance->createSwapchain(context.surface->getSurface(), context.surface->getWidth(), context.surface->getHeight());

			context.imageCount = context.swapchain->getImageCount();

			context.buffers.clear();
			context.buffers.resize(context.imageCount);

			device->allocateCommandBuffers(commandPool, &context.buffers[0], context.imageCount);

			contexts.at(i) = context;

		}		
	}
}

void RenderEngine::eventWindowDestroyed(GLFWwindow* window)
{
	for (int i = (int) contexts.size() - 1; i >= 0; i--)
	{
		if (contexts[i].window == window)
		{
			delete contexts[i].surface;
			delete contexts[i].swapchain;

			contexts[i].buffers.clear();
			contexts[i].buffers.shrink_to_fit();

			contexts.erase(contexts.begin() + i);
		}
	}
}

void RenderEngine::setWindowClearColor(GLFWwindow* window, glm::vec4 color)
{
	for (size_t i = 0; i < contexts.size(); i++)
	{
		RenderContext context = contexts.at(i);

		if (context.window == window)
		{
			context.clear_color.float32[0] = color.r;
			context.clear_color.float32[1] = color.g;
			context.clear_color.float32[2] = color.b;
			context.clear_color.float32[3] = color.a;

			contexts.at(i) = context;
		}
	}
}

void RenderEngine::recordBuffers()
{
	for (RenderContext context : contexts)
	{
		uint32_t image_count = context.imageCount;

		std::vector<VkImage> swapchainImages(image_count);

		if (vkGetSwapchainImagesKHR(*device->getDevice(), *context.swapchain->getSwapchain(), &image_count, &swapchainImages[0]) != VK_SUCCESS)
		{
			std::cout << "Could not get swapchain images!\n";
		}

		VkCommandBufferBeginInfo cmdBufferBeginInfo;
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.pNext = nullptr;
		cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		cmdBufferBeginInfo.pInheritanceInfo = nullptr;

		VkClearColorValue clear_color = context.clear_color;

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

			vkBeginCommandBuffer(context.buffers[i], &cmdBufferBeginInfo);

			vkCmdPipelineBarrier(
				context.buffers[i],
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &ptc
			);

			vkCmdClearColorImage(
				context.buffers[i],
				swapchainImages[i],
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				&clear_color,
				1,
				&isr
			);

			vkCmdPipelineBarrier(
				context.buffers[i],
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &ctp
			);

			if (vkEndCommandBuffer(context.buffers[i]) != VK_SUCCESS)
			{
				std::cout << "Could not record command buffers!\n";
			}
		}
	}
}

void RenderEngine::swapBuffers()
{
	for (RenderContext context : contexts)
	{
		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(*device->getDevice(), *context.swapchain->getSwapchain(),
			UINT64_MAX, *imageAvailable,
			VK_NULL_HANDLE, &image_index);
		switch (result)
		{
		case VK_SUCCESS:
			break;
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			std::cout << "Swapchain is out of date!\n";
			continue;
		default:
			std::cout << "Problem occurred during swap chain image acquisition!\n" << result << "\n";
			continue;
		}

		//submit a queue to the swap chain for rendering
		VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

		VkSubmitInfo subinfo;
		subinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subinfo.pNext = nullptr;
		subinfo.waitSemaphoreCount = 1;
		subinfo.pWaitSemaphores = imageAvailable;
		subinfo.pWaitDstStageMask = &wait_dst_stage_mask;
		subinfo.commandBufferCount = 1;
		subinfo.pCommandBuffers = &context.buffers[image_index];
		subinfo.signalSemaphoreCount = 1;
		subinfo.pSignalSemaphores = renderingFinished;

		if (vkQueueSubmit(*presentQueue, 1, &subinfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			std::cout << "Problem occured during swapchain image rendering!\n";
			continue;
		}

		//submit a queue to the swap chain for presentation
		VkPresentInfoKHR presinfo;
		presinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presinfo.pNext = nullptr;
		presinfo.waitSemaphoreCount = 1;
		presinfo.pWaitSemaphores = renderingFinished;
		presinfo.swapchainCount = 1;
		presinfo.pSwapchains = context.swapchain->getSwapchain();
		presinfo.pImageIndices = &image_index;
		presinfo.pResults = nullptr;

		result = vkQueuePresentKHR(*presentQueue, &presinfo);

		switch (result)
		{
		case VK_SUCCESS:
			break;
		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			std::cout << "Suboptimal KHR, Out of Date!\n";
			continue;
		default:
			std::cout << "Problem occurred during swapchain image presentation!\n";
			continue;
		}

	}
}

RenderEngine::~RenderEngine()
{
	vkQueueWaitIdle(*presentQueue);
	free(presentQueue);

	vkQueueWaitIdle(*graphicsQueue);
	free(graphicsQueue);

	device->destroySemaphore(imageAvailable);
	free(imageAvailable);

	device->destroySemaphore(renderingFinished);
	free(renderingFinished);

	device->destroyCommandPool(commandPool);
	free(commandPool);

	for (RenderContext context : contexts)
	{
		delete context.surface;

		context.buffers.clear();
		context.buffers.shrink_to_fit();

		delete context.swapchain;
	}

	contexts.clear();
	contexts.shrink_to_fit();

	delete instance;
}
