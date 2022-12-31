#include <vulkanloader.h>

bool ExtraVulkanFunctions::LoadVulkanFunctions(QVulkanInstance &vInstance)
{
	getSurfaceCapabilities = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vInstance.getInstanceProcAddr("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	getSurfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vInstance.getInstanceProcAddr("vkGetPhysicalDeviceSurfaceFormatsKHR");
	getSurfacePresentModes = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vInstance.getInstanceProcAddr("vkGetPhysicalDeviceSurfacePresentModesKHR");
	getSwapChain = (PFN_vkCreateSwapchainKHR)vInstance.getInstanceProcAddr("vkCreateSwapchainKHR");

	vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vInstance.getInstanceProcAddr("vkGetSwapchainImagesKHR");
	vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vInstance.getInstanceProcAddr("vkDestroySwapchainKHR");
	
	vkCreateImageView = (PFN_vkCreateImageView)vInstance.getInstanceProcAddr("vkCreateImageView");
	vkDestroyImageView = (PFN_vkDestroyImageView)vInstance.getInstanceProcAddr("vkDestroyImageView");

	return false;
}