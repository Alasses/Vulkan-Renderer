#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <QWindow>
#include <QDebug>
#include <QByteArray>

#include <vulkanloader.h>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentsFamily;
    int totalQueueCount { 0 };

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentsFamily.has_value();
    }
};

struct SwapChainSupportDetails 
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanWindow : public QWindow
{
public:
    VulkanWindow();
    ~VulkanWindow();

    void InitVulkan();

protected:
    void        CreateVulkanInstance();
    void        SetupWindowSurface();
    void        PreInitResources(); // Some decision before graphic initialization
    void        InitResources();    // Create graphics resources
    void        InitSwapChainResources();
    void        ReleaseSwapChainResources();
    void        ReleaseResources();
    void        StartNextFrame();


private:
    const QByteArrayList validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    QVulkanInstance         mVulkanInstance;

    VkSurfaceKHR            mWindowSurface;

    VkPhysicalDevice        mPhysicalDevice;
    VkDevice                mLogicalDevice;

    VkQueue                 mGraphicQueue;  // Drawing Queue
    VkQueue                 mPresentQueue;  // Present Queue
    VkCommandPool           mGraphicCommandPool;

    QVulkanFunctions*       mVulkanFunc;    // Normal Vulkan function access
    QVulkanDeviceFunctions* mDeviceFunc;    // Normal Vulkan function requires a logical device
    ExtraVulkanFunctions*   mExtraFunc;

    VkSwapchainKHR          mSwapChain;
    std::vector<VkImage>    mSwapChainImages;
    VkFormat                mSwapChainFormat;
    VkExtent2D              mSwapChainExtent;

    std::vector<VkImageView>    mSwapChainImageViews;

    /***** All Support Information Struct *****/

    SwapChainSupportDetails* mSwapChainSupportInfo { nullptr };  //  Used in two places
    

    /***** Some Other Parameters *****/

    float m_green = 0;

    void        SelectPhysicalDevice();
    bool        CheckPhysicalDevice(VkPhysicalDevice);
    bool        CheckPhysicaDeviceExtensionSupport(VkPhysicalDevice);
    SwapChainSupportDetails GetSwapChainSupport(VkPhysicalDevice);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice);

    void        CreateLogicalDeviceAndQueue();
    void        PrepareLogicalDeviceCreateStruct(VkDeviceCreateInfo&);

    /* SwapChain Creation */
    void                CreateSwapChain();
    VkSurfaceFormatKHR  ChooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR    ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D          ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    /* Image View Creation */
    void        CreateImageViews();

    /* Graphic Pipeline Creation */
    void        CreateGraphicsPipeline();
};

#endif // VULKANRENDERER_H
