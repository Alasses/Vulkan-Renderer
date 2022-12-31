#ifndef VULKANLOADER_H
#define VULKANLOADER_H

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <Windows.h>

#elif defined(VK_USE_PLATFORM_XCB_KHR)
#include <xcb/xcb.h>
#include <dlfcn.h>
#include <cstdlib>

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <cstdlib>

#endif

#include <QVulkanFunctions>
#include <QVulkanWindowRenderer>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

class ExtraVulkanFunctions
{
public:
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR getSurfaceCapabilities;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR getSurfaceFormats;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR getSurfacePresentModes;
	PFN_vkCreateSwapchainKHR getSwapChain;

	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;

	PFN_vkCreateImageView vkCreateImageView;
	PFN_vkDestroyImageView vkDestroyImageView;

	bool LoadVulkanFunctions(QVulkanInstance&);
};

#endif
