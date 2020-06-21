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

//�����ļ�
void SendFileThread::run()
{
    QFile*qfile=new QFile(*path);
    if(!qfile->open(QIODevice::ReadOnly)){
        QMessageBox::information(NULL, "��ʾ", "�ļ���ʧ��", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //ͷ���������ģ�
    QByteArray head;
    head.resize(sizeof(char)+sizeof(qint64)*2);
    char flag='f';
    qint64 sign=0;
    qint64 size=qfile->size();
    memcpy(head.data(), &flag, sizeof(char));

    //���Ϳ�ʼ��
    memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
    memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

    QByteArray start_raw;
    QString filename=path->section("/", -1, -1);
    start_raw.append(filename.toLocal8Bit());
    int s=start_raw.size();

    //����
    string encrypt=rsa->Encrypt(start_raw.data());
    QByteArray start_send(encrypt.c_str(), encrypt.length());
    start_send.insert(0, head);

    socket->writeDatagram(start_send, *groupAddr, groupPort);

    //�����м��
    QByteArray packet, rawData;
    rawData.resize(bufferSize);
    while(size>0){
        sign++;
        size-=bufferSize;

        //���ļ�
        qfile->read(rawData.data(), bufferSize);
        unsigned char*pdata=new unsigned char[bufferSize], *p=new unsigned char[512];
        memcpy(pdata, rawData.data(), bufferSize);

        //����
        uint64_t cypherSize;
        rsa->encryptBinary(pdata, bufferSize, p, cypherSize);

        QByteArray encryptedData;
        encryptedData.resize(cypherSize);
        memcpy(encryptedData.data(), p, cypherSize);

        delete [] pdata;
        delete [] p;

//        //���ܲ���
//        uint64_t plainSize;
//        unsigned char*pp=new unsigned char[256], *pd=new unsigned char[bufferSize];
//        memcpy(pp, encryptedData.data(), 256);
//        rsa.decryptBinary(pp, cypherSize, pd, plainSize);

//        QByteArray rec;
//        rec.resize(plainSize);
//        memcpy(rec.data(), pd, plainSize);


        //ͷ����
        memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
        memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

        //���
        packet.clear();
        packet.append(head);
        packet.append(encryptedData);

        socket->writeDatagram(packet, *groupAddr, groupPort);

        Sleep(2);
    }

    std::cout<<sign<<std::endl;

    //���ͽ�����
    QByteArray end;

    sign=-1;
    size=0;

    //ͷ����
    memcpy(head.data()+sizeof(char), &sign, sizeof(qint64));
    memcpy(head.data()+sizeof(char)+sizeof(qint64), &size, sizeof(qint64));

    end.append(head);

    socket->writeDatagram(end, *groupAddr, groupPort);

    //�ر��ļ�
    qfile->close();
}
