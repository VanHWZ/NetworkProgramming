#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("chatter");
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//加入多播组
void MainWindow::joinGroup()
{
    groupAddr=QHostAddress("224.0.0.2");
    groupPort=9999;

    multiSocket = new QUdpSocket();
    multiSocket->bind(QHostAddress::AnyIPv4, groupPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    multiSocket->joinMulticastGroup(groupAddr);

    connect(multiSocket, SIGNAL(readyRead()), this, SLOT(multiRecv()));
}

//获取公钥私钥
void MainWindow::getKeys()
{

//    //自己生成
//    rsa.GenerateRSAKey(2048);


    //从管理端获取
    uniSocket->writeDatagram(QString("k").toLocal8Bit(), managerAddr, managerPort);

    QByteArray recv;
    recv.resize(4096);
    int size=recv.size();
    while(!uniSocket->hasPendingDatagrams());
    uniSocket->readDatagram(recv.data(), recv.size());
    while(uniSocket->hasPendingDatagrams()){
        recv.resize(size+uniSocket->pendingDatagramSize());
        uniSocket->readDatagram(recv.data()+size, recv.size());
        size=recv.size();
    }
    id=recv[0];
    QString s=QString::fromLocal8Bit(recv.data()+1);
    QString pubk=s.section("spliterrrr", 0, 0);
    QString prik=s.section("spliterrrr", 1, 1);
    rsa.publicKey=pubk.toStdString();
    rsa.privateKey=prik.toStdString();
    std::cout<<"public key:"<<rsa.publicKey.size()<<std::endl<<"private key:"<<rsa.privateKey.size()<<std::endl;
    std::cout<<"getkey finished"<<std::endl;
}

//初始化
void MainWindow::init()
{
//    //设置解码
//    codec = QTextCodec::codecForName("utf8");

    //加入多播组
    joinGroup();

    //发送按钮
    connect(ui->sendPushButton, SIGNAL(clicked()), this, SLOT(sendMsg()));

    //单播socket
    managerAddr=QHostAddress("192.168.0.105");
    managerPort=9997;
    uniSocket=new QUdpSocket();
    uniSocket->bind(QHostAddress::AnyIPv4, 9998, QAbstractSocket::ReuseAddressHint);

    //rsa获取公钥密钥
    getKeys();

    connect(uniSocket, SIGNAL(readyRead()), this, SLOT(uniRecv()));

    //心跳线程，全局线程，设置窗口为父类
    heartbeatThread=new HeartbeatThread(this);
    heartbeatThread->setUniSocket(uniSocket);
    heartbeatThread->setMultiSocket(multiSocket);
    heartbeatThread->setId(id);
    heartbeatThread->setBeatMsg(ui->nicknameLineEdit->placeholderText());
    heartbeatThread->setUniDest(&managerAddr, managerPort);
    heartbeatThread->setMultiDest(&groupAddr, groupPort);
    heartbeatThread->start();

    //初始化昵称
    nickname=ui->nicknameLineEdit->placeholderText();

    //定时任务
    timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timedTask()));
    timer->start(2000);
}

//定时任务，检查用户是否还在线
void MainWindow::timedTask()
{
//    std::cout<<"In timedTask()"<<std::endl;
    if(idDict.empty())
        return;
    for(int i=1; i<=idDict.size(); i++)
    {
        if(idDict[i]!=0)
            idDict[i]++;
    }

    QString t;
    int tempid;
    for(int i=0;i<ui->memberListWidget->count();i++)
    {
        t=ui->memberListWidget->item(i)->text().section(" ", 0, 0);
        tempid=t.toInt();
        std::cout<<tempid<<":"<<idDict[tempid]<<std::endl;
        if(idDict[tempid]>5)
        {
            int row=ui->memberListWidget->row(ui->memberListWidget->item(i));
            ui->memberListWidget->takeItem(row);
            idDict[tempid]=0;
        }

    }
    timer->start(2000);
}

//接收单播信息
void MainWindow::uniRecv()
{
    QByteArray recvba;
    while(uniSocket->hasPendingDatagrams()){
        recvba.resize(uniSocket->pendingDatagramSize());
        uniSocket->readDatagram(recvba.data(), recvba.size());

        switch (recvba.front()) {
            case 'c':
                heartbeatThread->setTerminated();
                heartbeatThread->wait();
                this->close();
                break;
            default:
                break;
        }
    }
}

//多播接收，接收文件、文本等信息
void MainWindow::multiRecv()
{
    QByteArray recvba;
    while(multiSocket->hasPendingDatagrams()){
        recvba.resize(multiSocket->pendingDatagramSize());
        multiSocket->readDatagram(recvba.data(), recvba.size());

        switch (recvba.front()) {
            case 'm':
                recvba.remove(0, 1);
                recvMsg(recvba);
                break;
            case 'f':
                recvba.remove(0, 1);
                recvFile(recvba);
                break;
            case 'b':
                recvba.remove(0, 1);
                processBeat(recvba);
                break;
            default:
                break;
        }
    }
}

//接收文本聊天消息
void MainWindow::recvMsg(QByteArray ba)
{
    int nameLength;
    memcpy(&nameLength, ba.data(), sizeof(int));

    QByteArray nameba;
    nameba.resize(nameLength);
    memcpy(nameba.data(), ba.data()+sizeof(int), nameLength);

    QString name=QString::fromLocal8Bit(nameba);

    string decrypt=rsa.Decrypt(ba.data()+sizeof(int)+nameLength);
    QByteArray msg(decrypt.c_str(), decrypt.length());

    QString str;
    str+=name;
    str+=" "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    str+="\n";

    str+=QString::fromLocal8Bit(msg);
    str+='\n';

    ui->displayer->append(str);
}

//接收文件
void MainWindow::recvFile(QByteArray ba)
{
    //读取标号
    int offset=2*sizeof(qint64);
    qint64 sign;
    qint64 remainSize;
    memcpy(&sign, ba.data(), sizeof(qint64));
    memcpy(&remainSize, ba.data()+sizeof(qint64), sizeof(qint64));

    if(sign==0){
        string decyrpt=rsa.Decrypt(ba.data()+offset);
        QByteArray recv(decyrpt.c_str(), decyrpt.length());

        //接收开始包
        QString path=QString::fromLocal8Bit(recv);

        fileContent.clear();
        if(fileDialog!=NULL){
            delete fileDialog;
        }
        fileDialog=new FileDialog(this);
        fileDialog->setFileName(path.section("/", -1, -1));
        fileDialog->setTotalSize(remainSize);
        fileDialog->show();

        recvSize=0;

    }else if(sign==-1){
        //接收结束包
        QString fileName=fileDialog->getFileName();

        fileDialog->setRemainSize(0);

        QDir dir;
        std::cout<<dir.currentPath().data()<<std::endl;
        if(!dir.exists("recv"))
        {
            dir.mkdir("recv");
        }
        QFile*out=new QFile("./recv/"+fileName);
        if(!out->open(QIODevice::WriteOnly)){
            QMessageBox::information(NULL, "提示", "文件打开失败", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }

        int size=fileContent.size();
        for(int i=1;i<=size;i++)
        {
            out->write(fileContent[i]);
        }
        out->close();

        fileContent.clear();

        fileDialog->setPlayEnabled(true);

    }else{
        //接收中间包
        ba.remove(0, offset);
        QByteArray encryptedData(ba);

        //解密
        uint64_t plainSize, cypherSize=256;
        unsigned char*pRaw=new unsigned char[256], *pDecrypted=new unsigned char[128];
        memcpy(pRaw, encryptedData.data(), 256);
        rsa.decryptBinary(pRaw, cypherSize, pDecrypted, plainSize);

        QByteArray recv;
        recv.resize(plainSize);
        memcpy(recv.data(), pDecrypted, plainSize);

        fileContent[sign]=recv;

        delete [] pRaw;
        delete [] pDecrypted;

        fileDialog->setRemainSize(remainSize);

        recvSize+=128;
//        std::cout<<recvSize<<std::endl;
    }
}

//处理心跳信息
void MainWindow::processBeat(QByteArray ba)
{
    int id=ba[0];

    idDict[id]=1;

    //显示列表
    QString item=QString::number(id);
    item+=" ";
    item+=QString::fromLocal8Bit(ba.data()+1);

    int size=ui->memberListWidget->count();
    QString t;
    for(int i=0;i<size;i++){
        t=ui->memberListWidget->item(i)->text();
        if(t==item)
            return;
        if(t.front()==item.front()){
            ui->memberListWidget->item(i)->setText(item);
            return;
        }
    }
    QListWidgetItem tempItem;
    tempItem.setText(item);
    ui->memberListWidget->addItem(item);
}

//发送文本信息
void MainWindow::sendMsg()
{
    //获取文本
    QString msg;
    msg+=ui->inputTextEdit->toPlainText();

    QByteArray ba=msg.toLocal8Bit();
    int s=ba.size();
    if(s>214){
        QMessageBox::warning(this, "Warning!", "more than 214 bytes!", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        return;
    }
    ui->inputTextEdit->clear();

    //包头信息
    QByteArray head;
    head.resize(sizeof(char)+sizeof(int));
    char flag='m';
    QByteArray nameba=nickname.toLocal8Bit();
    int length=nameba.size();

    memcpy(head.data(), &flag, sizeof(char));
    memcpy(head.data()+sizeof(char), &length, sizeof(int));
    head.append(nameba);

    //加密
    string encrypt=rsa.Encrypt(msg.toLocal8Bit().data());
    QByteArray data(encrypt.c_str(), encrypt.length());

    //组合
    QByteArray send;
    send.append(head);
    send.append(data);

    //发送
    multiSocket->writeDatagram(send, groupAddr, groupPort);
}

void MainWindow::on_inputTextEdit_textChanged()
{
    QString text=ui->inputTextEdit->toPlainText();
    QByteArray ba=text.toLocal8Bit();
    int s=ba.size();
    QString label;
    label.prepend(QString::number(s));
    label+="/214";
    ui->countLabel->setText(label);
}

void MainWindow::on_nicknameLineEdit_textChanged(const QString &newname)
{
    if(newname.isEmpty()){
        nickname=ui->nicknameLineEdit->placeholderText();
        heartbeatThread->setBeatMsg(nickname);
        return;
    }
    heartbeatThread->setBeatMsg(newname);
    nickname=newname;
}

void MainWindow::on_pushButton_clicked()
{
    heartbeatThread->setTerminated();
    heartbeatThread->wait();
    this->close();
}

void MainWindow::on_selectPushButton_clicked()
{
    filedir=QFileDialog::getOpenFileName(this, "select mp4 or mp3", ".", "Images(*.mp3 *.mp4)");
    ui->lineEdit->setText(filedir.section("/", -1, -1));
    ui->sendFilePushButton->setEnabled(true);
}

//发送文件
void MainWindow::sendFile()
{
    sendFileThread=new SendFileThread(this);
    sendFileThread->setPath(&filedir);
    sendFileThread->setAddr(&groupAddr);
    sendFileThread->setPort(groupPort);
    sendFileThread->setSocket(multiSocket);
    sendFileThread->setRSA(&rsa);
    sendFileThread->start();
}

void MainWindow::on_sendFilePushButton_clicked()
{
    sendFile();
    ui->sendFilePushButton->setEnabled(false);
}
