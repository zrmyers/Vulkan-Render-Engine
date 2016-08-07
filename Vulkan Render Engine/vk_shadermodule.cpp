#include "vk_shadermodule.h"


VK_ShaderModule::VK_ShaderModule(std::string filename,VkDevice* device)
{
	VkResult result;

	device = VK_NULL_HANDLE;

	std::vector<char> file = Tools::load_binary_file_contents(filename);

	if (file.size() == 0)
	{
		return;
	}

	VkShaderModuleCreateInfo shader_module_create_info;
	shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_create_info.pNext = nullptr;
	shader_module_create_info.flags = 0;
	shader_module_create_info.codeSize = file.size();
	shader_module_create_info.pCode = (uint32_t*)&file[0];

	if ((result = vkCreateShaderModule(*device, &shader_module_create_info, nullptr, &shader)) != VK_SUCCESS)
	{
		std::cout << "Could not create shader module from file" << filename << "!\n";
		return;
	}

	this->device = device;
}

VkShaderModule* VK_ShaderModule::getShaderModule()
{
	if (device != VK_NULL_HANDLE)
	{
		return &shader;
	}

	return VK_NULL_HANDLE;
}

VK_ShaderModule::~VK_ShaderModule()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(*device, shader, nullptr);
	}
}