#pragma once
#include "common.h"

class VK_Instance;
class VK_Surface;
class VK_Swapchain;

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
public:
	RenderEngine(RenderEngineInfo*);
	~RenderEngine();

	void attachWindow(GLFWwindow*);

//private data
private:
	VK_Instance* instance;

	std::vector<GLFWwindow*> windows;
	std::vector<VK_Surface*> surfaces;
	std::vector<VK_Swapchain*> swapchains;
};

