#include "vk_renderer.h"

VK_Renderer::VK_Queue::VK_Queue(VK_QueueInfo* qinfo)
{
	queue = (VkQueue*)malloc(sizeof(VkQueue));
	vkGetDeviceQueue(*(qinfo->device), qinfo->queueFamilyIndex, qinfo->queueIndex, queue);
}

VK_Renderer::VK_Queue::~VK_Queue()
{
	if (queue != VK_NULL_HANDLE)
	{
		vkQueueWaitIdle(*queue);
		free((void*)queue);
	}
}