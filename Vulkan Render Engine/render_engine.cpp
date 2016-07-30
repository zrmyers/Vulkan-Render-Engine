#include "render_engine.h"
#include "vk_instance.h"
#include "vk_surface.h"
#include "vk_swapchain.h"

RenderEngine::RenderEngine(RenderEngineInfo* re_info)
{
	uint32_t requiredExtensionCount;
	const char** requiredExtensions;

	instance = nullptr;

	//put together required information for obtaining
	VK_InstanceInfo instanceInfo;
	instanceInfo.pApplicationName = re_info->applicationName.c_str();
	std::cout << re_info->applicationName << "\n";
	instanceInfo.applicationVersion = VK_MAKE_VERSION(re_info->app_VMAJOR,re_info->app_VMINOR,re_info->app_VPATCH);
	instanceInfo.pEngineName = "Vulkan Render Engine\n";
	instanceInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);
	instanceInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	
	//need instance level extensions for GLFW surface use
	requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	instanceInfo.ppEnabledExtensionNames = requiredExtensions;
	instanceInfo.enabledExtensionCount = requiredExtensionCount;

	//specify layers used.  Right now that is none
	instanceInfo.ppEnabledLayerNames = nullptr;
	instanceInfo.enabledLayerCount = 0;

	//we don't care about using our own allocation callbacks right now. set to null
	instanceInfo.allocs = nullptr;

	instance = new VK_Instance(&instanceInfo);

}

void RenderEngine::attachWindow(GLFWwindow* window)
{
	int width;
	int height;
	//add window to window collection
	windows.push_back(window);

	//create a surface for that window.
	VK_SurfaceInfo sinfo;
	sinfo.instance = instance->getInstance();
	sinfo.window = window;
	sinfo.allocs = nullptr;

	VK_Surface* surface = new VK_Surface(&sinfo);

	surfaces.push_back(surface);
	//we now have enough information to create a swapchain

	glfwGetWindowSize(window, &width, &height);

	VK_Swapchain* swapchain = instance->createSwapchain(surface->getSurface(),(uint32_t) width,(uint32_t) height);

	swapchains.push_back(swapchain);
}

RenderEngine::~RenderEngine()
{
	for (VK_Surface* surface : surfaces)
	{
		delete surface;
	}
	surfaces.clear();
	surfaces.shrink_to_fit();

	windows.clear();
	windows.shrink_to_fit();

	delete instance;
}
