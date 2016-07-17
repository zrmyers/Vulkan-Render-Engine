#pragma once
//includes
#include <vulkan\vulkan.h>
#include <GLFW\glfw3.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

typedef struct VK_RendererInfo
{
	//determined at application level
	std::string applicationName;
	int app_VMAJOR;
	int app_VMINOR;
	int app_VPATCH;
	//determined at engine level
	std::string engineName;
	int eng_VMAJOR;
	int eng_VMINOR;
	int eng_VPATCH;
	GLFWwindow* window;
	uint32_t screen_width;
	uint32_t screen_height;
}VK_RendererInfo;

class VK_Renderer
{
private:
	//private types
	typedef struct VK_InstanceInfo
	{
		//determined at application level
		const char* pApplicationName; 
		uint32_t applicationVersion;
		//determined at engine level
		const char* pEngineName; 
		uint32_t engineVersion;		
		//determined at renderer level
		uint32_t apiVersion;
		uint32_t enabledLayerCount; 
		const char* const* ppEnabledLayerNames;
		uint32_t enabledExtensionCount;
		const char* const* ppEnabledExtensionNames;
		VkAllocationCallbacks* allocs;
	}VK_InstanceInfo;
	typedef struct VK_PhysicalDeviceInfo
	{
		//determined at renderer level
		VkInstance* instance; 
		uint32_t minAPIVersionMajor;
		uint32_t minAPIVersionMinor;
		uint32_t minAPIVersionPatch;
		uint32_t maxDimm2D;
	}VK_PhysicalDeviceInfo;
	typedef struct VK_SurfaceInfo
	{
		VkInstance* instance;
		GLFWwindow* window;
		VkAllocationCallbacks* allocs;
	}VK_SurfaceInfo;
	typedef struct VK_DeviceInfo
	{
		VkPhysicalDevice* physicalDevice;
		uint32_t graphicsQueueFamilyIndex;
		uint32_t presentQueueFamilyIndex;
		uint32_t enabledLayerCount;
		const char* const* ppEnabledLayerNames;
		uint32_t enabledExtensionCount;
		const char* const* ppEnabledExtensionNames;
		VkPhysicalDeviceFeatures* pEnabledFeatures;
		VkAllocationCallbacks* allocs;
	}VK_DeviceInfo;
	typedef struct VK_SwapchainInfo
	{
		uint32_t screen_width;
		uint32_t screen_height;
		VkPhysicalDevice* physicalDevice;
		VkSurfaceKHR* surface;
		VkDevice* device;
		VkAllocationCallbacks* allocs;
	}VK_SwapchainInfo;
private:
	//VK_instance class maintains lifecycle of vkInstance
	class VK_Instance
	{
	public:
		VK_Instance(VK_InstanceInfo*);
		~VK_Instance();

		VkInstance* getInstance();	
	private:
		VkInstance* instance;
		VkAllocationCallbacks* allocs;

		bool checkExtensionAvailability(const char*, std::vector<VkExtensionProperties>);
	};
	//VK_Surface class maintains lifecycle of vkSurface
	class VK_Surface
	{
	public:
		VK_Surface(VK_SurfaceInfo*);
		~VK_Surface();

		VkSurfaceKHR* getSurface();
	private:
		VkSurfaceKHR* surface;
		VkAllocationCallbacks* allocs;
		VkInstance* instance;
	};
	//VK_PhysicalDevice class maintains lifecycle of vkPhysicalDevice
	class VK_PhysicalDevice
	{
	public:
		VK_PhysicalDevice(VK_PhysicalDeviceInfo*);
		~VK_PhysicalDevice();

		VkPhysicalDevice* getPhysicalDevice();
		uint32_t getGraphicsQueueFamilyIndex();
		uint32_t getPresentationQueueFamilyIndex(VkSurfaceKHR*);
	private:
		bool checkProperties(VkPhysicalDevice,VK_PhysicalDeviceInfo*);
	private:
		VkPhysicalDevice* physicalDevice;
		uint32_t graphics_queue_family_index;
		uint32_t presentation_queue_family_index;
	};
	//VK_Device class maintains lifecycle of vkDevice
	class VK_Device
	{
	public:
		VK_Device(VK_DeviceInfo*);
		~VK_Device();

		VkDevice* getDevice();
		VkQueue* getPresentQueue();
		VkQueue* getGraphicsQueue();
		VkSemaphore* getImageAvailableSemaphore();
		VkSemaphore* getRenderingFinishedSemaphore();
	private:
		VkDevice* device;
		VkAllocationCallbacks* allocs;
		VkQueue* graphicsQueue;
		VkQueue* presentQueue;
		VkSemaphore* imageAvailableSemaphore;
		VkSemaphore* renderingFinishedSemaphore;

		uint32_t presentQueueFamilyIndex;
		uint32_t graphicsQueueFamilyIndex;
	};
	//VK_SwapChain class maintains lifecycle of vkSwapchain
	class VK_Swapchain
	{
	public:
		VK_Swapchain(VK_SwapchainInfo*);
		~VK_Swapchain();

		VkSwapchainKHR* getSwapchain();
	private:
		VkSwapchainKHR* swapchain;
		VkAllocationCallbacks* allocs;
		VkDevice* device;
	};
public:
	VK_Renderer(VK_RendererInfo*);
	~VK_Renderer();

	bool Draw();
private:
	VK_Instance* instance;					//vulkan instance
	VK_Surface* surface;					//vulkan surface
	VK_PhysicalDevice* physicalDevice;		//vulkan physical device
	VK_Device* device;						//vulkan device
	VK_Swapchain* swapchain;                //vulkan swapchain	
	VkAllocationCallbacks* allocs;          //vulkan allocation callbacks

	bool SubmitQueue();
	bool PresentQueue(uint32_t*);
};
