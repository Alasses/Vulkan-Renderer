#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, VulkanWindow *w)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VulkanWindow *vWindow = new VulkanWindow();

    //QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    QWidget *wrapper = QWidget::createWindowContainer(vWindow);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(wrapper, 0, 0, 7, 2);

    ui->vWidget->setLayout(layout);

    wrapper->resize(800, 600);
    wrapper->show();

    //w.resize(800, 600);
    //w.show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

