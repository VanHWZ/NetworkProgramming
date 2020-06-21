#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QMap>
#include <iostream>
#include "rsa.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void multiRecv();
    void uniRecv();
    void timedTask();

private slots:
    void on_deletePushButton_clicked();

    void on_displayer_itemClicked(QListWidgetItem *item);

private:
    void joinGroup();
    void getKeys();
    void sendKeys();
    void init();
    void processBeat(QByteArray);
    int getNewId();

    MyRSA rsa;
    QUdpSocket*multiSocket, *uniSocket;
    quint16 uniport;
    QTextCodec*codec;

    QMap<int, int> idDict;

    QHostAddress targetAddr;
    quint16 targetPort;

    QListWidgetItem *selectedItem;
    Ui::MainWindow *ui;

    QTimer*timer;
};

#endif // MAINWINDOW_H
