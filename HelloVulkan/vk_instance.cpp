#include "vk_renderer.h"

VK_Renderer::VK_Instance::VK_Instance(VK_InstanceInfo* instInfo)
{
	//allocate necessary memory
	VkResult result;
	instance = (VkInstance*)malloc(sizeof(VkInstance));

	//get application info
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = "Hello Vulkan";//instInfo->pApplicationName;  // must obtain from elsewhere
	app_info.applicationVersion = instInfo->applicationVersion; //must obtain from elsewhere
	app_info.pEngineName = instInfo->pEngineName;  //must obtain from elsewhere
	app_info.engineVersion = instInfo->engineVersion; // must obtain elsewhere
	app_info.apiVersion = instInfo->apiVersion; // must obtain elsewhere

	//lets make sure indicated extensions are available
	uint32_t ext_count = 0;
	if (((vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr)) != VK_SUCCESS) || (ext_count == 0))
	{
		std::cout << "Error occured during instance extension enumeration!\n";
		return;
	}

	std::vector<VkExtensionProperties> available_ext(ext_count);
	if (vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, &available_ext[0]) != VK_SUCCESS)
	{
		std::cout << "Error occurred during instance extensions enumerations!\n";
		return;
	}

	for (size_t i = 0; i < instInfo->enabledExtensionCount; i++)
	{
		if (!checkExtensionAvailability((instInfo->ppEnabledExtensionNames)[i], available_ext))
		{
			std::cout << "Could not find instance extension named \"" << (instInfo->ppEnabledExtensionNames)[i] << "\"!\n";
			instance = VK_NULL_HANDLE;
			return;
		}
	}

	//get instance create info
	VkInstanceCreateInfo inst_info;
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = nullptr;
	inst_info.flags = 0; //reserved for future use
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledLayerCount = instInfo->enabledLayerCount;  //must obtain elsewhere
	inst_info.ppEnabledLayerNames = instInfo->ppEnabledLayerNames; //must obtain elsewhere
	inst_info.enabledExtensionCount = instInfo->enabledExtensionCount; //must obtain elsewhere
	inst_info.ppEnabledExtensionNames = instInfo->ppEnabledExtensionNames; //must obtain elsewhere

	allocs = instInfo->allocs; // must obtain elsewhere 

	if ((result = vkCreateInstance(&inst_info, allocs, instance)) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to create vulkan instance!\n";
	}
}

bool VK_Renderer::VK_Instance::checkExtensionAvailability(const char* extension_name, std::vector<VkExtensionProperties> available_extensions)
{
	for (size_t i = 0; i < available_extensions.size(); i++)
	{
		if (strcmp(available_extensions[i].extensionName, extension_name) == 0)
		{
			return true;
		}
	}
	return false;
}

VkInstance* VK_Renderer::VK_Instance::getInstance()
{
	return instance;
}

VK_Renderer::VK_Instance::~VK_Instance()
{
	vkDestroyInstance(*instance, allocs);
	free((void*)instance);
}