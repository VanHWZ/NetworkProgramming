#include "heartbeatthread.h"

HeartbeatThread::HeartbeatThread(QWidget *parent)
{
    terminated=true;
}

HeartbeatThread::~HeartbeatThread()
{

}

void HeartbeatThread::setUniSocket(QUdpSocket *s)
{
    uniSocket = s;
}

void HeartbeatThread::setMultiSocket(QUdpSocket *s)
{
    multiSocket=s;
}

void HeartbeatThread::setBeatMsg(QString m)
{
    beatMsg.clear();
    beatMsg='b';
    beatMsg.append(id);
    beatMsg.append(m);
}

void HeartbeatThread::setTerminated()
{
    terminated=false;
}

void HeartbeatThread::run()
{
    while (terminated && uniSocket!=nullptr && !beatMsg.isEmpty()) {
        sleep(2);
        uniSocket->writeDatagram(beatMsg.toLocal8Bit(), *managerAddr, managerPort);
        multiSocket->writeDatagram(beatMsg.toLocal8Bit(), *groupAddr, groupPort);
    }
}

void HeartbeatThread::setUniDest(QHostAddress *addr, quint16 port)
{
    managerAddr=addr;
    managerPort=port;
}

void HeartbeatThread::setMultiDest(QHostAddress *addr, quint16 port)
{
    groupAddr=addr;
    groupPort=port;
}

void HeartbeatThread::setId(int i)
{
    id=i;
}
