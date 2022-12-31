#include "vulkanrenderer.h"
#include <optional>
#include <vector>
#include <set>

#define dbStream qDebug()

VulkanWindow::VulkanWindow()
{
    setSurfaceType(VulkanSurface);

    create();

    InitVulkan();

    ReleaseResources();
}

VulkanWindow::~VulkanWindow()
{

    delete mExtraFunc;

    mDeviceFunc->vkDestroyDevice(mLogicalDevice, nullptr);

    //  Destroy the window surface
    

    //  Destroy the Vulkan instance associate with this window
    mVulkanInstance.destroy();
}

void VulkanWindow::InitVulkan()
{
    CreateVulkanInstance();
    SetupWindowSurface();
    PreInitResources();
}

// Create and bind the Vulkan Instance with current Window
void VulkanWindow::CreateVulkanInstance()
{
    mVulkanInstance.setLayers(validationLayers);
    mVulkanInstance.setApiVersion(QVersionNumber(1, 3));
    qDebug() << "Supported Vulkan Version: " << mVulkanInstance.supportedApiVersion().toString();

    if (!mVulkanInstance.create())
        qFatal("Failed to create Vulkan instance: %d", mVulkanInstance.errorCode());
    this->setVulkanInstance(&mVulkanInstance);

    mExtraFunc = new ExtraVulkanFunctions();
    mExtraFunc->LoadVulkanFunctions(mVulkanInstance);
}

void VulkanWindow::SetupWindowSurface()
{
    this->setSurfaceType(QSurface::VulkanSurface);
    mWindowSurface = mVulkanInstance.surfaceForWindow(this);
}

void VulkanWindow::PreInitResources()
{
    //General Vulkan function not involve logical device
    mVulkanFunc = mVulkanInstance.functions();

    SelectPhysicalDevice();

    CreateLogicalDeviceAndQueue();

    CreateSwapChain();

    CreateImageViews();

    CreateGraphicsPipeline();
}

void VulkanWindow::CreateSwapChain()
{
    if (mSwapChainSupportInfo == nullptr)
    {
        mSwapChainSupportInfo = new SwapChainSupportDetails();
        *mSwapChainSupportInfo = GetSwapChainSupport(mPhysicalDevice);
    }

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapChainSurfaceFormat(mSwapChainSupportInfo->formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(mSwapChainSupportInfo->presentModes);
    mSwapChainExtent = ChooseSwapExtent(mSwapChainSupportInfo->capabilities);

    uint32_t imageCount = mSwapChainSupportInfo->capabilities.minImageCount + 1;
    if (mSwapChainSupportInfo->capabilities.maxImageCount > 0 &&
        imageCount > mSwapChainSupportInfo->capabilities.maxImageCount)
    {
        imageCount = mSwapChainSupportInfo->capabilities.maxImageCount;
    }

    //  Construct the create info struct for creating swap chain
    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mWindowSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = mSwapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;    //  Directly rendered

    //  In case the graphic queue is different from presenting queue,
    //  we will want the image in swap chain to transfer between each queue explicitly.
    QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentsFamily.value() };
    if (indices.graphicsFamily != indices.presentsFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = mSwapChainSupportInfo->capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (mExtraFunc->getSwapChain(mLogicalDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    //  Retrieving swap chain images
    mExtraFunc->vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, nullptr);
    mSwapChainImages.resize(imageCount);
    mExtraFunc->vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, mSwapChainImages.data());

    //  Save swap chain detail
    mSwapChainFormat = surfaceFormat.format;
}

void VulkanWindow::CreateImageViews()
{
    mSwapChainImageViews.resize(mSwapChainImages.size());

    for (size_t i = 0; i < mSwapChainImages.size(); i++) 
    {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mSwapChainFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (mExtraFunc->vkCreateImageView(mLogicalDevice, &createInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create image views!");
        }
    }
}

void VulkanWindow::CreateGraphicsPipeline()
{

}

void VulkanWindow::InitResources()
{
    
}

void VulkanWindow::ReleaseResources()
{
    //  Release swap chain image view
    for (auto imageView : mSwapChainImageViews)
    {
        mExtraFunc->vkDestroyImageView(mLogicalDevice, imageView, nullptr);
    }

    //  Release swap chain
    mExtraFunc->vkDestroySwapchainKHR(mLogicalDevice, mSwapChain, nullptr);
}

void VulkanWindow::InitSwapChainResources()
{

}

void VulkanWindow::ReleaseSwapChainResources()
{
   
}

void VulkanWindow::StartNextFrame()
{
    //m_green += 0.005f;
    //if (m_green > 1.0f)
    //    m_green = 0.0f;

    //vkclearcolorvalue clearcolor = {{ 0.0f, m_green, 0.0f, 1.0f }};
    //vkcleardepthstencilvalue cleards = { 1.0f, 0 };
    //vkclearvalue clearvalues[2];
    //memset(clearvalues, 0, sizeof(clearvalues));
    //clearvalues[0].color = clearcolor;
    //clearvalues[1].depthstencil = cleards;

    //vkrenderpassbegininfo rpbegininfo;
    //memset(&rpbegininfo, 0, sizeof(rpbegininfo));
    //rpbegininfo.stype = vk_structure_type_render_pass_begin_info;
    //rpbegininfo.renderpass = mvulkanwindow->defaultrenderpass();
    //rpbegininfo.framebuffer = mvulkanwindow->currentframebuffer();
    //const qsize sz = mvulkanwindow->swapchainimagesize();
    //rpbegininfo.renderarea.extent.width = sz.width();
    //rpbegininfo.renderarea.extent.height = sz.height();
    //rpbegininfo.clearvaluecount = 2;
    //rpbegininfo.pclearvalues = clearvalues;
    //vkcommandbuffer cmdbuf = mvulkanwindow->currentcommandbuffer();
    //mdevicefunc.vkcmdbeginrenderpass(cmdbuf, &rpbegininfo, vk_subpass_contents_inline);

    //mdevicefunc.vkcmdendrenderpass(cmdbuf);

    //mvulkanwindow->frameready();
    //mvulkanwindow->requestupdate(); // render continuously, throttled by the presentation rate

}

// Select the physical devices by requirements
void VulkanWindow::SelectPhysicalDevice()
{
    qDebug() << "\n***** GPU info *****";

    //Get all the available physical device
    uint32_t physicalDeviceCount = 0;
    mVulkanFunc->vkEnumeratePhysicalDevices(mVulkanInstance.vkInstance(), &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);
    mVulkanFunc->vkEnumeratePhysicalDevices(mVulkanInstance.vkInstance(), &physicalDeviceCount, physicalDeviceList.data());

    uint32_t selection = 0;
    for (uint32_t i = 0; i < physicalDeviceList.size(); i++)
    {
        VkPhysicalDevice pDevice = physicalDeviceList[i];

        //Retrive device properties and features
        VkPhysicalDeviceProperties deviceProperties;
        mVulkanFunc->vkGetPhysicalDeviceProperties(pDevice, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        mVulkanFunc->vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

        qDebug("Number %d device", i);

        if (CheckPhysicalDevice(pDevice))
        {
            selection = i;
        }
    }

    //Set the physical device
    mPhysicalDevice = physicalDeviceList[selection];
}

// Check the given physical device
bool VulkanWindow::CheckPhysicalDevice(VkPhysicalDevice pDevice)
{
    const char * DeviceTypes[5] = {
        "Other", "Integrated GPU", "Discrete GPU", "Virtual GPU", "CPU"
    };

    //Retrive device properties and features
    VkPhysicalDeviceProperties deviceProperties;
    mVulkanFunc->vkGetPhysicalDeviceProperties(pDevice, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    mVulkanFunc->vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

    qDebug("Device Name: %s", deviceProperties.deviceName);
    qDebug("Device Type: %s", DeviceTypes[deviceProperties.deviceType]);

    //Looking for discrete gpu
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        //Check the features
        dbStream << "Device geometry shader support: " << (deviceFeatures.geometryShader ? "true" : "false");
        dbStream << "Device tessellation shader support: " << (deviceFeatures.tessellationShader ? "true" : "false");
        dbStream << "Device multi-viewport support: " << (deviceFeatures.multiViewport ? "true" : "false");
        if (deviceFeatures.geometryShader && deviceFeatures.tessellationShader && deviceFeatures.multiViewport)
        {
            QueueFamilyIndices indices = FindQueueFamilies(pDevice);
            if (indices.isComplete())
            {
                dbStream << "Device graphic and present queue support: True";
                if (CheckPhysicaDeviceExtensionSupport(pDevice))
                {
                    dbStream << "Device Extension support: True";
                    if (mSwapChainSupportInfo == nullptr)
                    {
                        mSwapChainSupportInfo = new SwapChainSupportDetails();
                    }
                    *mSwapChainSupportInfo = GetSwapChainSupport(pDevice);
                    if (!mSwapChainSupportInfo->formats.empty() && 
                        !mSwapChainSupportInfo->presentModes.empty())
                    {
                        dbStream << "Device SwapChain support: True";
                        return true;
                    }
                }
            }
        }
    }

    qDebug("Physical device doesn't support all features.");
    return false;
}

bool VulkanWindow::CheckPhysicaDeviceExtensionSupport(VkPhysicalDevice pDevice)
{
    uint32_t extensionCount;
    mVulkanFunc->vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    mVulkanFunc->vkEnumerateDeviceExtensionProperties(pDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanWindow::GetSwapChainSupport(VkPhysicalDevice pDevice)
{
    SwapChainSupportDetails details;

    mExtraFunc->getSurfaceCapabilities(pDevice, mWindowSurface, &details.capabilities);

    uint32_t formatCount;
    mExtraFunc->getSurfaceFormats(pDevice, mWindowSurface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        mExtraFunc->getSurfaceFormats(pDevice, mWindowSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    mExtraFunc->getSurfacePresentModes(pDevice, mWindowSurface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        mExtraFunc->getSurfacePresentModes(pDevice, mWindowSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

// Check the queue families supported by the device
QueueFamilyIndices VulkanWindow::FindQueueFamilies(VkPhysicalDevice pDevice)
{
    // We want to distinguish fail 0 and index value 0
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    mVulkanFunc->vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    mVulkanFunc->vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

    indices.totalQueueCount = queueFamilies.size();
    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {     
        if (!indices.graphicsFamily.has_value() && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        if (!indices.presentsFamily.has_value() && mVulkanInstance.supportsPresent(pDevice, i, this))
        {
            indices.presentsFamily = i;
        }
        ++i;
    }

    return indices;
}

void VulkanWindow::CreateLogicalDeviceAndQueue()
{
    VkDeviceCreateInfo createInfo {};

    // Create info for both queues (graphic and present)
    QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> queueFamiliesNeedToCreate = { indices.graphicsFamily.value(), indices.presentsFamily.value() };
    for (uint32_t queueFamily : queueFamiliesNeedToCreate)
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures;
    mVulkanFunc->vkGetPhysicalDeviceFeatures(mPhysicalDevice, &deviceFeatures);

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledLayerCount = 0;

    //  Enabling all the device extensions, i.e. SwapChain
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (mVulkanFunc->vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) != VK_SUCCESS)
    {
        dbStream << "== Fail to create Vulkan Device!";
    }

    // Store the quick access of Vulkan device functions, Qt feature
    mDeviceFunc = mVulkanInstance.deviceFunctions(mLogicalDevice);

    // Get queues
    mDeviceFunc->vkGetDeviceQueue(mLogicalDevice, indices.graphicsFamily.value(), 0, &mGraphicQueue);
    mDeviceFunc->vkGetDeviceQueue(mLogicalDevice, indices.presentsFamily.value(), 0, &mPresentQueue);
}

/***** SwapChain Creation *****/

VkSurfaceFormatKHR VulkanWindow::ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanWindow::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//  The extent of the swapchain is its resolution, usually equals to the number of pixels of the render window
VkExtent2D VulkanWindow::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
    {
        return capabilities.currentExtent;
    }
    else 
    {
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(800),
            static_cast<uint32_t>(600)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
