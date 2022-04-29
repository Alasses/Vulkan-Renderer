#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <QVulkanWindowRenderer>
#include <QVulkanWindow>
#include <QVulkanFunctions>
#include <QDebug>
#include <QByteArray>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow();
    ~VulkanWindow();
    QVulkanWindowRenderer *createRenderer() override;

private:
    const QByteArrayList validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    QVulkanInstance vInstance;

    //bool checkValidationLayerSupport();
};

class VulkanRenderer : public QVulkanWindowRenderer
{
public:
    VulkanRenderer(VulkanWindow *window);

    //
    //  Some virtual functions, Qt build in pipeline
    //

    void preInitResources() override;           //Some decision before graphic initialization
    void initResources() override;              //Create graphics resources
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

private:

    const QByteArrayList deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VulkanWindow *vWindow;
    QVulkanInstance *vInstance;
    VkSurfaceKHR windowSurface;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    VkQueue graphicQueue;
    VkCommandPool graphicCommandPool;

    QVulkanFunctions *vFunc;            //Normal Vulkan function access
    QVulkanDeviceFunctions *deviceFunc; //Normal Vulkan function requires a logical device

    float m_green = 0;

    void selectPhysicalDevice();
};

#endif // VULKANRENDERER_H
