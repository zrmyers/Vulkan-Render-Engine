#include "vk_renderer.h"

//VK_Instance class definition
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

VkInstance* VK_Renderer::VK_Instance::getInstance()
{
	return instance;
}

VK_Renderer::VK_Instance::~VK_Instance()
{
	vkDestroyInstance(*instance, allocs);
}

VK_Renderer::VK_Renderer(VK_RendererInfo* rinfo)
{
	//information to create instance object
	VK_InstanceInfo instInfo;	
	instInfo.pApplicationName = rinfo->applicationName.c_str();
	instInfo.applicationVersion = VK_MAKE_VERSION(rinfo->app_VMAJOR, rinfo->app_VMINOR, rinfo->app_VPATCH);
	instInfo.pEngineName = rinfo->engineName.c_str();
	instInfo.enabledExtensionCount = 0;
	instInfo.ppEnabledExtensionNames = nullptr;
	instInfo.enabledLayerCount = 0;
	instInfo.ppEnabledLayerNames = nullptr;
	instInfo.allocs = nullptr;
	instInfo.apiVersion = 0; //current version of vulkan

	//create instance object
	if ((instance = new VK_Instance(&instInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create vulkan instance!\n";
	}
}

VK_Renderer::~VK_Renderer()
{
	if (instance != nullptr) delete instance;
}