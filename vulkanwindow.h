#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include <QVulkanWindowRenderer>
#include <QVulkanWindow>

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow();
    QVulkanWindowRenderer *createRenderer() override;

private:
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkInstance vInstance;
};

#endif // VULKANWINDOW_H
