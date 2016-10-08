#ifndef VK_SHADER_MODULE_H
#define VK_SHADER_MODULE_H

#include "common.h"

class VK_ShaderModule
{
public:
	VK_ShaderModule(std::string, VkDevice*);
	~VK_ShaderModule();

	VkShaderModule* getShaderModule();

private:
	VkShaderModule shader;
	VkDevice* device;
};

#endif // !VK_SHADER_MODULE_H

