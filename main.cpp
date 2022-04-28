#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*
    QVulkanInstance vInstance;
    vInstance.setLayers({ "VK_LAYER_KHRONOS_validation" });
    if (!vInstance.create())
        qFatal("Failed to create Vulkan instance: %d", vInstance.errorCode());
    */


    //VulkanWindow *vWindow = new VulkanWindow();
    //vWindow->setVulkanInstance(&vInstance);


    MainWindow w(nullptr, nullptr);
    w.show();
    return a.exec();
}
