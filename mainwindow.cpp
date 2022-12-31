#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, VulkanWindow *w)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mVulkanWindow = new VulkanWindow();

    QWidget *wrapper = QWidget::createWindowContainer(mVulkanWindow);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(wrapper, 0, 0, 7, 2);

    ui->vWidget->setLayout(layout);

    wrapper->resize(ui->vWidget->width(), ui->vWidget->height());
    //wrapper->resize(1920, 1200);
    //wrapper->show();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete mVulkanWindow;
}

