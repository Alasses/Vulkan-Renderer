#include "vulkanrenderer.h"

VulkanWindow::VulkanWindow()
{
    vInstance.setLayers({ "VK_LAYER_KHRONOS_validation" });
    if (!vInstance.create())
        qFatal("Failed to create Vulkan instance: %d", vInstance.errorCode());

    this->setVulkanInstance(&vInstance);
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}

VulkanRenderer::VulkanRenderer(VulkanWindow *w)
{
    vWindow = w;
}

void VulkanRenderer::initResources()
{
    deviceFunc = vWindow->vulkanInstance()->deviceFunctions(vWindow->device());
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
