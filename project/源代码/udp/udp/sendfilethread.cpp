#include "sendfilethread.h"

SendFileThread::SendFileThread(QObject*parent)
{
    this->setParent(parent);
    bufferSize=128;
}

SendFileThread::~SendFileThread()
{

}

void SendFileThread::setPath(QString *p)
{
    path=p;
}

void SendFileThread::setAddr(QHostAddress *a)
{
    groupAddr=a;
}

void SendFileThread::setPort(quint16 p)
{
    groupPort=p;
}

void SendFileThread::setSocket(QUdpSocket *s)
{
    socket=s;
}

void SendFileThread::setRSA(MyRSA *r)
{
    rsa=r;
}

//发送文件
void SendFileThread::run()
{
    QFile*qfile=new QFile(*path);
    if(!qfile->open(QIODevice::ReadOnly)){
        QMessageBox::information(NULL, "提示", "文件打开失败", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //头参数（明文）
    QByteArray head;
    head.resize(sizeof(char)+sizeof(qint64)*2);
    char flag='f';
    qint64 sign=0;
    qint64 size=qfile->size();
    memcpy(head.data(), &flag, sizeof(char));

    //发送开始包
    memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
    memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

    QByteArray start_raw;
    QString filename=path->section("/", -1, -1);
    start_raw.append(filename.toLocal8Bit());
    int s=start_raw.size();

    //加密
    string encrypt=rsa->Encrypt(start_raw.data());
    QByteArray start_send(encrypt.c_str(), encrypt.length());
    start_send.insert(0, head);

    socket->writeDatagram(start_send, *groupAddr, groupPort);

    //发送中间包
    QByteArray packet, rawData;
    rawData.resize(bufferSize);
    while(size>0){
        sign++;
        size-=bufferSize;

        //读文件
        qfile->read(rawData.data(), bufferSize);
        unsigned char*pdata=new unsigned char[bufferSize], *p=new unsigned char[512];
        memcpy(pdata, rawData.data(), bufferSize);

        //加密
        uint64_t cypherSize;
        rsa->encryptBinary(pdata, bufferSize, p, cypherSize);

        QByteArray encryptedData;
        encryptedData.resize(cypherSize);
        memcpy(encryptedData.data(), p, cypherSize);

        delete [] pdata;
        delete [] p;

//        //解密测试
//        uint64_t plainSize;
//        unsigned char*pp=new unsigned char[256], *pd=new unsigned char[bufferSize];
//        memcpy(pp, encryptedData.data(), 256);
//        rsa.decryptBinary(pp, cypherSize, pd, plainSize);

//        QByteArray rec;
//        rec.resize(plainSize);
//        memcpy(rec.data(), pd, plainSize);


        //头参数
        memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
        memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

        //组合
        packet.clear();
        packet.append(head);
        packet.append(encryptedData);

        socket->writeDatagram(packet, *groupAddr, groupPort);

        Sleep(2);
    }

    std::cout<<sign<<std::endl;

    //发送结束包
    QByteArray end;

    sign=-1;
    size=0;

    //头参数
    memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
    memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

    end.append(head);

    socket->writeDatagram(end, *groupAddr, groupPort);

    //关闭文件
    qfile->close();
}
