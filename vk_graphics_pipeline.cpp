#include "vk_graphics_pipeline.h"

VK_GraphicsPipeline::VK_GraphicsPipeline(VK_GraphicsPipelineInfo* pipe_info)
{
	allocs = pipe_info->allocs;

	//create the renderpass attachment description
	VkAttachmentDescription attachment_Description;
	attachment_Description.flags = 0;
	attachment_Description.format = pipe_info->format;
	attachment_Description.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_Description.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachment_Description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//create the subpass attachment description
	VkAttachmentReference color_attachment_reference;
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description;
	subpass_description.flags = 0;
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.inputAttachmentCount = 0;
	subpass_description.pInputAttachments = nullptr;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachment_reference;
	subpass_description.pResolveAttachments = nullptr;
	subpass_description.pDepthStencilAttachment = nullptr;
	subpass_description.preserveAttachmentCount = 0;
	subpass_description.pPreserveAttachments = nullptr;

	//create the renderpass creation info
	VkRenderPassCreateInfo render_pass_creation_info;
	render_pass_creation_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_creation_info.pNext = nullptr;
	render_pass_creation_info.attachmentCount = 1;
	render_pass_creation_info.pAttachments = &attachment_Description;
	render_pass_creation_info.subpassCount = 1;
	render_pass_creation_info.pSubpasses = &subpass_description;
	render_pass_creation_info.dependencyCount = 0;
	render_pass_creation_info.pDependencies = nullptr;

	if (vkCreateRenderPass(*pipe_info->device, &render_pass_creation_info, allocs, &renderpass) != VK_SUCCESS)
	{
		std::cout << "Could not create a render pass!\n";
	}
}

VkRenderPass* VK_GraphicsPipeline::getRenderPass()
{
	return &renderpass;
}

VK_GraphicsPipeline::~VK_GraphicsPipeline()
{
	vkDestroyRenderPass(*device, renderpass, allocs);
}

