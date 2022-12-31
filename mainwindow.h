#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "vulkanrenderer.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, VulkanWindow *w = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    VulkanWindow* mVulkanWindow{ nullptr };
    QVulkanInstance mVulkanInstance;

    void SetupVulkanWindow();

};
#endif // MAINWINDOW_H
