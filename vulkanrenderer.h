#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <QVulkanWindowRenderer>
#include <QVulkanWindow>
#include <QVulkanFunctions>
#include <QDebug>

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow();
    QVulkanWindowRenderer *createRenderer() override;

private:
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    QVulkanInstance vInstance;
};

class VulkanRenderer : public QVulkanWindowRenderer
{
public:
    VulkanRenderer(VulkanWindow *w);

    //
    //  Some virtual functions, Qt build in pipeline
    //

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

private:

    VulkanWindow *vWindow;
    QVulkanDeviceFunctions *deviceFunc;

    float m_green = 0;
};

#endif // VULKANRENDERER_H
