#pragma once
//includes
#include <vulkan\vulkan.h>
#include <stdlib.h>
#include <iostream>
#include <string>

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
	};
	//VK_Surface class maintains lifecycle of vkSurface
	class VK_Surface
	{
	public:
		VK_Surface();
		~VK_Surface();

		VkSurfaceKHR* getSurface();
	private:
		VkSurfaceKHR* surface;
	};
	//VK_PhysicalDevice class maintains lifecycle of vkPhysicalDevice
	class VK_PhysicalDevice
	{
	public:
		VK_PhysicalDevice();
		~VK_PhysicalDevice();

		VkPhysicalDevice* getPhysicalDevice();
	private:
		VkPhysicalDevice* physicalDevice;
	};
	//VK_Device class maintains lifecycle of vkDevice
	class VK_Device
	{
	public:
		VK_Device();
		~VK_Device();

		VkDevice* getDevice();
	private:
		VkDevice* device;
	};
public:
	VK_Renderer(VK_RendererInfo*);
	~VK_Renderer();

private:
	VK_Instance* instance;					//vulkan instance
	VK_Surface* surface;					//vulkan surface
	VK_PhysicalDevice* physicalDevice;		//vulkan physical device
	VK_Device* device;						//vulkan device

};
