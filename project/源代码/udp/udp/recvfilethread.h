#ifndef RECVFILETHREAD_H
#define RECVFILETHREAD_H

#include <QThread>
#include <QUdpSocket>
#include <QMessageBox>
#include <QMutex>
#include "rsa.h"
#include "filedialog.h"


class RecvFileThread : public QThread
{
    Q_OBJECT

public:
    RecvFileThread(QObject*parent=nullptr);
    ~RecvFileThread();

    void run();
    void setSocket(QUdpSocket*);
    void setRSA(MyRSA*);
    void setFileContent(QMap<qint64, QByteArray>*);
    void setByteArray(QByteArray);
    void setFileDialog(FileDialog*);


signals:
    void setFileName(QString);
    void setTotalSize(qint64);
    void setRemainSize(qint64);
    void setPlayEnabled(bool);
    void display();

private:
    QString basePath;
    QUdpSocket*socket;
    MyRSA*rsa;
    FileDialog*fileDialog;
    QMap<qint64, QByteArray>*fileContent;
    QByteArray ba;
    QMutex mutex;

};

#endif // RECVFILETHREAD_H
