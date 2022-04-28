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

VulkanRenderer::VulkanRenderer(VulkanWindow *w)
{
    //Store the window instance and Qt device function for Vulkan
    vWindow = w;
    vInstance = vWindow->vulkanInstance();
}

void VulkanRenderer::preInitResources()
{
    //Check and select the physical devices by requirements
    selectPhysicalDevice();
}

void VulkanRenderer::initResources()
{
    //Store the quick access of Vulkan functions (both)
    vFunc = vInstance->functions();
    deviceFunc = vInstance->deviceFunctions(vWindow->device());
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
    //Check all the avaliable device, and select the physical device
    const char * DeviceTypes[5] = {
        "Other", "Integrated GPU", "Discrete GPU", "Virtual GPU", "CPU"
    };
    qDebug("\n***** GPU info *****");
    auto dList = vWindow->availablePhysicalDevices();
    uint32_t selection = 0;
    for(uint32_t i = 0; i < dList.length(); i++)
    {
        auto pDevice = vWindow->availablePhysicalDevices()[i];
        qDebug("# %d device", i);
        qDebug("Device Name: %s", pDevice.deviceName);
        qDebug("Device Type: %s\n", DeviceTypes[pDevice.deviceType]);

        //Looking for discrete gpu
        if(pDevice.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            selection = i;
    }
    vWindow->setPhysicalDeviceIndex(selection);

    //Store the physical device
    physicalDevice = vWindow->physicalDevice();
}
