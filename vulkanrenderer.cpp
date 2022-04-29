#include "vulkanrenderer.h"

VulkanWindow::VulkanWindow()
{
    //Should stays the same as class variable validationLayers
    vInstance.setLayers(validationLayers);

    //Create the Vulkan instance, and bind to this window
    if (!vInstance.create())
        qFatal("Failed to create Vulkan instance: %d", vInstance.errorCode());
    this->setVulkanInstance(&vInstance);
}

VulkanWindow::~VulkanWindow()
{
    //Destroy the Vulkan instance associate with this window
    vInstance.destroy();
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}

VulkanRenderer::VulkanRenderer(VulkanWindow *window)
{
    //Store the window and Vulkan instance
    vWindow = window;
    vInstance = vWindow->vulkanInstance();

    //Store(create) the window surface
    windowSurface = vInstance->surfaceForWindow(vWindow);
}

void VulkanRenderer::preInitResources()
{
    //General Vulkan function not involve logical device
    vFunc = vInstance->functions();

    //Check and select the physical devices by requirements
    selectPhysicalDevice();

    //Set the logical device extensions
    vWindow->setDeviceExtensions(deviceExtensions);
}

void VulkanRenderer::initResources()
{
    //Store the quick access of Vulkan device functions
    deviceFunc = vInstance->deviceFunctions(vWindow->device());

    //Store the logical device, created by Qt
    logicalDevice = vWindow->device();

    //Store the graphic queue and command pool
    graphicQueue = vWindow->graphicsQueue();
    graphicCommandPool = vWindow->graphicsCommandPool();
}

void VulkanRenderer::releaseResources()
{

}

void VulkanRenderer::initSwapChainResources()
{

}

void VulkanRenderer::releaseSwapChainResources()
{

}

void VulkanRenderer::startNextFrame()
{

    m_green += 0.005f;
    if (m_green > 1.0f)
        m_green = 0.0f;

    VkClearColorValue clearColor = {{ 0.0f, m_green, 0.0f, 1.0f }};
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = vWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = vWindow->currentFramebuffer();
    const QSize sz = vWindow->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuf = vWindow->currentCommandBuffer();
    deviceFunc->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    deviceFunc->vkCmdEndRenderPass(cmdBuf);

    vWindow->frameReady();
    vWindow->requestUpdate(); // render continuously, throttled by the presentation rate

}

//
//  According to documentation, for physical device features, the QVulkanWindow class
//  simply enables all available features from VK_1.0 of vkGetPhysicalDeviceFeatures.
//
void VulkanRenderer::selectPhysicalDevice()
{
    qDebug("\n***** GPU info *****");
    const char * DeviceTypes[5] = {
        "Other", "Integrated GPU", "Discrete GPU", "Virtual GPU", "CPU"
    };

    //Get all the available physical device
    uint32_t physicalDeviceCount = 0;
    vFunc->vkEnumeratePhysicalDevices(vInstance->vkInstance(), &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDeviceList(physicalDeviceCount);
    vFunc->vkEnumeratePhysicalDevices(vInstance->vkInstance(), &physicalDeviceCount, physicalDeviceList.data());
    auto dList = vWindow->availablePhysicalDevices();

    //Check each device
    uint32_t selection = 0;
    for(uint32_t i = 0; i < physicalDeviceList.size(); i++)
    {
        VkPhysicalDevice pDevice = physicalDeviceList[i];

        //Retrive device properties and features
        VkPhysicalDeviceProperties deviceProperties;
        vFunc->vkGetPhysicalDeviceProperties(pDevice, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vFunc->vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

        qDebug("Number %d device", i);
        qDebug("Device Name: %s", deviceProperties.deviceName);
        qDebug("Device Type: %s", DeviceTypes[deviceProperties.deviceType]);

        //Looking for discrete gpu
        if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            //Check the features
            qDebug("Device geometry shader support: %s", deviceFeatures.geometryShader ? "true" : "false");
            qDebug("Device tessellation shader support: %s", deviceFeatures.tessellationShader ? "true" : "false");
            qDebug("Device multi-viewport support: %s", deviceFeatures.multiViewport ? "true" : "false");
            if(deviceFeatures.geometryShader && deviceFeatures.tessellationShader && deviceFeatures.multiViewport)
            {
                //Get the Queue Families
                uint32_t queueFamilyCount = 0;
                vFunc->vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, nullptr);
                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
                vFunc->vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilies.data());

                //Check the Queue Family, and make final decision
                bool supportGraphicQueue = false;
                uint32_t graphicQueueCount = 0;
                for(const auto & queueFamily : queueFamilies)
                {
                    //Check if is the Graphic Queue
                    if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                     {
                        supportGraphicQueue = true;
                        graphicQueueCount = queueFamily.queueCount;
                    }
                }
                if(supportGraphicQueue)
                {
                    qDebug("Device Graphic Queue support: true, Queue Count: %d", graphicQueueCount);

                    //Check if the order is correct, since we are using VK function not Qt function
                    if(dList[i].deviceID == deviceProperties.deviceID)
                        selection = i;
                    else
                        qDebug("Vk Qt physical device order not matching!");

                    break;
                }
            }
        }
    }

    //Set the physical device
    vWindow->setPhysicalDeviceIndex(selection);
    physicalDevice = vWindow->physicalDevice();

    qDebug("");
}
