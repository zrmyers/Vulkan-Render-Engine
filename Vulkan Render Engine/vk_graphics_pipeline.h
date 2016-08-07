#ifndef VK_GRAPHICS_PIPELINE_H
#define VK_GRAPHICS_PIPELINE_H

#include "common.h"

typedef struct VK_GraphicsPipelineInfo
{
	VkFormat format;
	VkDevice* device;
	VkAllocationCallbacks* allocs;
}VK_GraphicsPipelineInfo;

class VK_GraphicsPipeline
{
public:
	VK_GraphicsPipeline(VK_GraphicsPipelineInfo*);
	~VK_GraphicsPipeline();

	VkRenderPass* getRenderPass();

private:
	VkAllocationCallbacks* allocs;
	VkRenderPass renderpass;
	VkDevice* device;
};

#endif