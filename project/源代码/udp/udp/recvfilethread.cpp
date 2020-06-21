#include "recvfilethread.h"

RecvFileThread::RecvFileThread(QObject*parent)
{
    this->setParent(parent);
    basePath=QString("./recv/");
}

RecvFileThread::~RecvFileThread()
{

}

void RecvFileThread::setSocket(QUdpSocket *s)
{
    socket=s;
}

void RecvFileThread::setRSA(MyRSA *r)
{
    rsa=r;
}

void RecvFileThread::setFileContent(QMap<qint64, QByteArray> *m)
{
    fileContent=m;
}

void RecvFileThread::setByteArray(QByteArray b)
{
    ba=b;
}

void RecvFileThread::setFileDialog(FileDialog *fd)
{
    fileDialog=fd;
}

void RecvFileThread::run()
{
    int offset=2*sizeof(qint64);
    qint64 sign;
    qint64 remainSize;
    memcpy(&sign, ba.data(), sizeof(qint64));
    memcpy(&remainSize, ba.data()+sizeof(qint64), sizeof(qint64));

    if(sign==0){
        string decyrpt=rsa->Decrypt(ba.data()+offset);
        QByteArray recv(decyrpt.c_str(), decyrpt.length());

        //接收开始包
        QString path=QString::fromLocal8Bit(recv);

        fileContent->clear();

        emit setFileName(path.section("/", -1, -1));
        emit setPlayEnabled(false);
        emit setTotalSize(remainSize);
        emit display();

    }else if(sign==-1){
        //接收结束包
        QString fileName=fileDialog->getFileName();

        emit setRemainSize(0);

        QFile*out=new QFile("./recv/"+fileName);
        if(!out->open(QIODevice::WriteOnly)){
            QMessageBox::information(NULL, "提示", "文件打开失败", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        int size=fileContent->size();
        std::cout<<size<<std::endl;
        for(int i=1;i<=size;i++)
        {
            std::cout<<i<<std::endl;
            out->write((*fileContent)[i]);
        }
        out->close();
        std::cout<<-1<<std::endl;

        emit setPlayEnabled(true);
        disconnect(fileDialog);

    }else{
        //接收中间包
        ba.remove(0, offset);
        QByteArray encryptedData(ba);

        //解密
        uint64_t plainSize, cypherSize=256;
        unsigned char*pRaw=new unsigned char[256], *pDecrypted=new unsigned char[128];
        memcpy(pRaw, encryptedData.data(), 256);
        rsa->decryptBinary(pRaw, cypherSize, pDecrypted, plainSize);

        QByteArray recv;
        recv.resize(plainSize);
        memcpy(recv.data(), pDecrypted, plainSize);

        delete [] pRaw;
        delete [] pDecrypted;

        mutex.lock();
        (*fileContent)[sign]=recv;
        emit setRemainSize(remainSize);
        mutex.unlock();

//        std::cout<<sign<<std::endl;
    }
//    exit(0);
}
