#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QTextCodec>
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QMap>
#include <iostream>
#include <windows.h>
#include "rsa.h"
#include "heartbeatthread.h"
#include "sendfilethread.h"
#include "filedialog.h"


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
    void sendMsg();
    void timedTask();

private slots:
    void on_inputTextEdit_textChanged();

    void on_nicknameLineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_selectPushButton_clicked();

    void on_sendFilePushButton_clicked();


private:
    void init();
    void joinGroup();
    void getKeys();
    void sendFile();
    void recvFile(QByteArray);
    void recvMsg(QByteArray);
    void processBeat(QByteArray);


    MyRSA rsa;
    QUdpSocket*multiSocket,*uniSocket;
    QTextCodec*codec;
    QDateTime*dateTime;
    HeartbeatThread*heartbeatThread;

    QHostAddress managerAddr, groupAddr;
    quint16 managerPort, groupPort;

    int id;

    Ui::MainWindow *ui;

    SendFileThread*sendFileThread=NULL;
//    RecvFileThread*recvFileThread=NULL;

    FileDialog*fileDialog=NULL;
    QString filedir;
    QMap<qint64, QByteArray> fileContent;
    qint64 recvSize;

    QString nickname;

    QMap<int, int> idDict;
    QTimer*timer;

};

#endif // MAINWINDOW_H
