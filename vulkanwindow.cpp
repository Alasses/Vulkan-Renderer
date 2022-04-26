#include "vulkanwindow.h"

VulkanWindow::VulkanWindow()
{

}


QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}
