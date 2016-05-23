#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QImage getImage();

    QImage image;

private:
    Ui::MainWindow *ui;

    QUrl url;
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;
    QByteArray jpegData;
    QPixmap pixmap;
};

#endif // MAINWINDOW_H
