#ifndef SENDFILE_H
#define SENDFILE_H

#include <QThread>
#include <QHostAddress>
#include <QUdpSocket>
#include <QFile>
#include <QMessageBox>
#include <windows.h>
#include "rsa.h"

class SendFileThread : public QThread
{
public:
    SendFileThread(QObject*parent=nullptr);
    ~SendFileThread();

    void run();
    void setPath(QString*);
    void setAddr(QHostAddress*);
    void setPort(quint16);
    void setSocket(QUdpSocket*);
    void setRSA(MyRSA*);

private:
    QString*path;
    QHostAddress*groupAddr;
    quint16 groupPort;
    QUdpSocket*socket;
    MyRSA*rsa;
    int bufferSize;

};

#endif // SENDFILE_H
