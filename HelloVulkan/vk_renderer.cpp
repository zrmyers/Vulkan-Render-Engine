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

VK_Renderer::VK_PhysicalDevice::VK_PhysicalDevice(VK_PhysicalDeviceInfo* physInfo)
{
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice* physicalDevices;

	physicalDevice = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice));
	
	if ((result = vkEnumeratePhysicalDevices(*(physInfo->instance), &physicalDeviceCount, nullptr)) != VK_SUCCESS)
	{
		std::cout << "VK Error: Failed to find physical devices!\n";
		physicalDevice = VK_NULL_HANDLE;
		return;
	}
	
	if (physicalDeviceCount < 1)
	{
		std::cout << "VK Error: No vulkan compatible devices exist!\n";
		physicalDevice = VK_NULL_HANDLE;
		return;
	}
	else
	{
		physicalDevices = (VkPhysicalDevice*)malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));

		if ((result = vkEnumeratePhysicalDevices(*(physInfo->instance), &physicalDeviceCount, physicalDevices)) != VK_SUCCESS)
		{
			std::cout << "VK Error: No vulkan compatible devices found!\n";
			physicalDevice = VK_NULL_HANDLE;
			return;
		}
		//pick the device with correct specs
		for (uint32_t i = 0; i < physicalDeviceCount; i++)
		{
			if (checkProperties(physicalDevices[i],physInfo))
			{
				*physicalDevice = physicalDevices[i];
				break;
			}
		}

		free((void*)physicalDevices);
	}
}

bool VK_Renderer::VK_PhysicalDevice::checkProperties(VkPhysicalDevice phydev,VK_PhysicalDeviceInfo* info)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(phydev, &properties);
	vkGetPhysicalDeviceFeatures(phydev, &features);

	//is the proper vulkan api version supported?
	uint32_t api_major = VK_VERSION_MAJOR(properties.apiVersion);
	uint32_t api_minor = VK_VERSION_MINOR(properties.apiVersion);
	uint32_t api_patch = VK_VERSION_PATCH(properties.apiVersion);

	//make sure maximal screen dimmensions are supported as well as minimal api is supported
	if ((api_major < info->minAPIVersionMajor ) && 
		(api_minor < info->minAPIVersionMinor ) &&
		(api_patch < info->minAPIVersionPatch ) &&
		(properties.limits.maxImageDimension2D < info->maxDimm2D))
	{
		std::cout << "Physical device " << phydev << " does not support required parameters!" << std::endl;
		return false;
	}

	//make sure device has a graphics queue family
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(phydev, &queue_family_count, nullptr);

	if (queue_family_count == 0)
	{
		std::cout << "Physical device " << phydev << " has no queue families!\n";
		return false;
	}

	std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(phydev, &queue_family_count, &queue_family_properties[0]);

	for (uint32_t i = 0; i < queue_family_count; i++)
	{
		if ((queue_family_properties[i].queueCount > 0) &&
			(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			graphics_queue_family_index = i;
			return true;
		}
	}

	return false;
}

uint32_t VK_Renderer::VK_PhysicalDevice::getGraphicsQueueFamilyIndex()
{
	return graphics_queue_family_index;
}

VkPhysicalDevice* VK_Renderer::VK_PhysicalDevice::getPhysicalDevice()
{
	return physicalDevice;
}

VK_Renderer::VK_PhysicalDevice::~VK_PhysicalDevice()
{
	free((void*) physicalDevice);
}

VK_Renderer::VK_Device::VK_Device(VK_DeviceInfo* devinfo)
{	
	if ((devinfo->physicalDevice) == VK_NULL_HANDLE)
	{
		std::cout << "Vulkan Error: Failed to create appropriate vulkan device!\n";
		device = VK_NULL_HANDLE;
		return;
	}

	device = (VkDevice*)malloc(sizeof(VkDevice));
	float queuePriorities = 1.0f; //this may be renderer level information

	//data for queue creation with device
	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = devinfo->graphicsQueueFamilyIndex; //need to fill in with data from physical device
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriorities;

	//data for device creation
	VkDeviceCreateInfo devCreateInfo;
	devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.pNext = nullptr;
	devCreateInfo.flags = 0;
	devCreateInfo.queueCreateInfoCount = 1;
	devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	devCreateInfo.enabledLayerCount = devinfo->enabledLayerCount;
	devCreateInfo.ppEnabledLayerNames = devinfo->ppEnabledLayerNames;
	devCreateInfo.enabledExtensionCount = devinfo->enabledExtensionCount;
	devCreateInfo.ppEnabledExtensionNames = devinfo->ppEnabledExtensionNames;
	devCreateInfo.pEnabledFeatures = devinfo->pEnabledFeatures;

	allocs = devinfo->allocs;

	if (vkCreateDevice(*(devinfo->physicalDevice), &devCreateInfo, allocs, device ) != VK_SUCCESS)
	{
		std::cout << "Vulkan Error: Could Not create Vulkan device!\n";
		device = VK_NULL_HANDLE;
	}
}

VkDevice* VK_Renderer::VK_Device::getDevice()
{
	return device;
}

VK_Renderer::VK_Device::~VK_Device()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(*device);
		vkDestroyDevice(*device, allocs);
	}	
}

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

	//information to create logical device object
	VK_DeviceInfo devInfo;
	devInfo.physicalDevice = physicalDevice->getPhysicalDevice();
	devInfo.graphicsQueueFamilyIndex = physicalDevice->getGraphicsQueueFamilyIndex();
	devInfo.enabledExtensionCount = 0;
	devInfo.ppEnabledExtensionNames = nullptr;
	devInfo.enabledLayerCount = 0;
	devInfo.ppEnabledLayerNames = nullptr;
	devInfo.pEnabledFeatures = nullptr;
	devInfo.allocs = nullptr;

	//create logical device object
	std::cout << "Creating Vulkan Device Instance\n";
	if ((device = new VK_Device(&devInfo)) == nullptr)
	{
		std::cout << "Error: Failed to create logical device!\n";
		return;
	}

	//information to create graphics queue
	VK_QueueInfo qinfo;
	qinfo.device = device->getDevice();
	qinfo.queueFamilyIndex = physicalDevice->getGraphicsQueueFamilyIndex();
	qinfo.queueIndex = 0;

	//create queue instance
	std::cout << "Creating Vulkan Graphics Queue Instance\n";
	if ((queue = new VK_Queue(&qinfo)) == nullptr)
	{
		std::cout << "Error: Failed to create queue!\n";
		return;
	}
}

VK_Renderer::~VK_Renderer()
{
	std::cout << "Destroying Vulkan Graphics Queue Instance\n";
	if (queue != nullptr) delete queue;
	std::cout << "Destroying Vulkan Device Instance\n";
	if (device != nullptr) delete device;
	std::cout << "Destroying Vulkan Physical Device Instance\n";
	if (physicalDevice != nullptr) delete physicalDevice;
	std::cout << "Destroying Vulkan Instance\n";
	if (instance != nullptr) delete instance;
}