#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), url("http://192.168.8.1:8083/?action=snapshot")
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QImage MainWindow::getImage()
{
    do {
        reply = manager.get(QNetworkRequest(url));

        //请求结束并下载完成后，退出子事件循环
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        //开启子事件循环
        loop.exec();

        jpegData = reply->readAll();

    } while (!pixmap.loadFromData(jpegData));

    image.loadFromData(jpegData);

    return image;
}

