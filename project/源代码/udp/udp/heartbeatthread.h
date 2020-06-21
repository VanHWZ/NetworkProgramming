#ifndef HEARTBEATTHREAD_H
#define HEARTBEATTHREAD_H
#include <QThread>
#include <QUdpSocket>
#include <QHostAddress>


class HeartbeatThread: public QThread
{
    Q_OBJECT
public:
    explicit HeartbeatThread(QWidget *parent = nullptr);
    ~HeartbeatThread();

    void run();
    void setBeatMsg(QString);
    void setUniSocket(QUdpSocket*);
    void setMultiSocket(QUdpSocket*);
    void setUniDest(QHostAddress*addr, quint16 port);
    void setMultiDest(QHostAddress*addr, quint16 port);
    void setTerminated();
    void setId(int);

private:
    QString beatMsg;
    QUdpSocket*uniSocket, *multiSocket;
    QHostAddress*managerAddr, *groupAddr;
    quint16 managerPort, groupPort;
    int id;
    bool terminated;
};

#endif // HEARTBEATTHREAD_H
