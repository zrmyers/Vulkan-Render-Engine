#include "vk_instance.h"


VK_Instance::VK_Instance(VK_InstanceInfo* instInfo)
{
	//allocate necessary memory
	VkResult result;

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

	if ((result = vkCreateInstance(&inst_info, allocs, &instance)) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to create vulkan instance!\n";
		return;
	}

	this->apiversion = instInfo->apiVersion;

	physicalDevice = nullptr;
	device = nullptr;

	return;
}

VkInstance* VK_Instance::getInstance()
{
	return &instance;
}

VK_Swapchain* VK_Instance::createSwapchain(VkSurfaceKHR* surface,uint32_t screen_width,uint32_t screen_height)
{
	if (device == nullptr)
	{
		//we need to create a logical device
		findDevice(surface);
	}

	VK_Swapchain* swapchain;
	//information to create swap chain object
	VK_SwapchainInfo swapInfo;
	swapInfo.physicalDevice = physicalDevice->getPhysicalDevice();
	swapInfo.surface = surface;
	swapInfo.screen_width = screen_width;
	swapInfo.screen_height = screen_height;
	swapInfo.device = device->getDevice();
	swapInfo.allocs = nullptr;

	//create swap chain object
	std::cout << "Creating Vulkan Swapchain Instance\n";
	if ((swapchain = new VK_Swapchain(&swapInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create swapchain!\n";
	}

	return swapchain;
}

bool VK_Instance::checkExtensionAvailability(const char* extension_name, std::vector<VkExtensionProperties> available_extensions)
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

void VK_Instance::findDevice(VkSurfaceKHR* surface)
{
	VK_PhysicalDeviceInfo phys_info;
	VK_DeviceInfo dev_info;
	std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	phys_info.instance = &this->instance;
	phys_info.apiversion = apiversion;
	phys_info.maxDimm2D = 4096;  //this may need to come from somewhere else
	
	//find an appropriate physical device
	physicalDevice = new VK_PhysicalDevice(&phys_info);

	//now create a device
	dev_info.physicalDevice = physicalDevice->getPhysicalDevice();
	dev_info.graphicsQueueFamilyIndex = physicalDevice->getGraphicsQueueFamilyIndex();
	dev_info.presentQueueFamilyIndex = physicalDevice->getPresentationQueueFamilyIndex(surface);
	dev_info.enabledLayerCount = 0;
	dev_info.ppEnabledLayerNames = nullptr;
	dev_info.enabledExtensionCount = (uint32_t) extensions.size();
	dev_info.ppEnabledExtensionNames = &extensions[0];
	dev_info.pEnabledFeatures = nullptr;
	dev_info.allocs = allocs;
	
	device = new VK_Device(&dev_info);

	return;
}

VK_Device* VK_Instance::getDevice()
{
	return device;
}

VK_Instance::~VK_Instance()
{
	delete device;
	delete physicalDevice;
	vkDestroyInstance(instance, allocs);
}