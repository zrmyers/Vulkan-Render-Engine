#pragma once
#include "common.h"
#include <glm.hpp>

class VK_Instance;
class VK_Surface;
class VK_Swapchain;
class VK_Device;

typedef struct RenderEngineInfo
{
	//determined at application level
	std::string applicationName;
	int app_VMAJOR;
	int app_VMINOR;
	int app_VPATCH;
}RenderEngineInfo;

class RenderEngine
{
//private data types
private:
	typedef struct RenderContext
	{
		GLFWwindow* window;
		VK_Surface* surface;
		VK_Swapchain* swapchain;
		uint32_t imageCount;
		std::vector<VkCommandBuffer> buffers; //when multithreading, this will need to be local for each thread
		VkClearColorValue clear_color;
	}RenderContext;
//public methods
public:
	RenderEngine(RenderEngineInfo*);
	~RenderEngine();

	void attachWindow(GLFWwindow*);

	void setWindowClearColor(GLFWwindow*,glm::vec4);

	void pollWindowResize();
	void eventWindowDestroyed(GLFWwindow*);

	void recordBuffers();
	void swapBuffers();

//private data
private:
	VK_Instance* instance;
	VK_Device* device;

	std::vector<RenderContext> contexts;

	uint32_t presentQueueFamilyIndex;
	uint32_t graphicsQueueFamilyIndex; //may need to indicate a queue count for each in the future

	VkCommandPool* commandPool;
	VkQueue* presentQueue;
	VkQueue* graphicsQueue;
	VkSemaphore* imageAvailable;
	VkSemaphore* renderingFinished;
};

