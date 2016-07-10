# HelloVulkan
Project utilizing Vulkan API

# Introduction

The purpose of this project is to get familiar with the Vulkan API, as well as to establish a reusable class (wrappper) for interacting
with the GPU in future projects.

This project was developed with insight gleaned from "API Without Secrets" tutorial series by Pawel L. on Intel's devleoper zone, to which a link is provided below.  The vulkan specification document is also a handy tool.

#Project Description

##Environment

###Windows, Visual Studio 2015 Community
The visual studio project files contain information about linking, include directories, and resolve dependencies.  The vulkan-1.lib from LunarG sdk, as well as glfw libraries are linked against.  
##Classes
Brief description of each class.
###VK_Renderer
Encapsulates interactions between vulkan objects.  Is primarily responsible for initiating Vulkan, and handling draw commands.
###VK_Instance
Vulkance instance wrapper.  Handles creation and destruction of vulkan instance.
###VK_PhysicalDevice
Vulkan physical device wrapper.  Performs logic associated with finding a physical device which support correct vulkan renderer specs.
###VK_Device
Vulkan device wrapper.  Creates and destroys a vulkan device instance, as well as the proper queues as determined from the physical device.
###VK_Queue
Vulkan Queue wrapper.  Obtains queue from device, and controls access to queue.
###VK_Surface
Vulkan Surface wrapper.  Maintains a Vulkan Surface.
#Useful Links
https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-1  
https://github.com/KhronosGroup/Vulkan-Docs  
https://vulkan.lunarg.com  
http://www.glfw.org/docs/3.2/vulkan.html  
