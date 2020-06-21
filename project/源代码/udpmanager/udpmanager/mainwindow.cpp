#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("manager");
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//����ಥ��
void MainWindow::joinGroup()
{
    multiSocket = new QUdpSocket();
    multiSocket->bind(QHostAddress::AnyIPv4, 9999, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    multiSocket->joinMulticastGroup(QHostAddress("224.0.0.2"));

    connect(multiSocket, SIGNAL(readyRead()), this, SLOT(multiRecv()));
}

//��ȡ��Կ˽Կ
void MainWindow::getKeys()
{
    rsa.GenerateRSAKey(2048);
//    std::cout<<"public key:"<<rsa.publicKey<<endl;
//    std::cout<<"private key:"<<rsa.privateKey<<endl;
}

//�ಥ���Ľ���
void MainWindow::multiRecv()
{
    QByteArray recvba;
    while(multiSocket->hasPendingDatagrams()){
        recvba.resize(multiSocket->pendingDatagramSize());
        multiSocket->readDatagram(recvba.data(), recvba.size());

        switch (recvba.front()) {
            case 'f':
                break;
            case 'b':
                processBeat(recvba.data()+1);
            default:
                break;
        }
    }
}

//�������Ľ���
void MainWindow::uniRecv()
{
    QByteArray recvba;
    while(uniSocket->hasPendingDatagrams()){
        recvba.resize(uniSocket->pendingDatagramSize());
        uniSocket->readDatagram(recvba.data(), recvba.size(), &targetAddr, &targetPort);

        switch (recvba.front()) {
            case 'k':
                sendKeys();
                break;
            case 'b':
//                processBeat(recvba.data()+1);
                break;
            default:
                break;
        }
    }
}

//��ȡһ���µ�id
int MainWindow::getNewId()
{
    for(int j=1;j<=idDict.size();j++){
        if(idDict[j]==0)
            return j;
    }
    int i=idDict.size()+1;
    return i;
}

//������Կ��ͬʱ����id
void MainWindow::sendKeys(){
    std::cout<<"sendkey()"<<std::endl;
    QString send;
    send+=QString::fromStdString(rsa.publicKey);
    send+="spliterrrr";
    send+=QString::fromStdString(rsa.privateKey);

    QByteArray msg;
    int newId=getNewId();
    idDict[newId]=1;
    msg.append(newId);
    msg.append(send.toLocal8Bit());
    int size=msg.size(), i=0, buff=4096;
    while(i<size){
        uniSocket->writeDatagram(msg.data()+i, buff, targetAddr, targetPort);
        i+=1024;
    }
    if(i!=size){
        uniSocket->writeDatagram(msg.data()+i-1024, size-i, targetAddr, targetPort);
    }
}

//����������Ϣ
void MainWindow::processBeat(QByteArray ba)
{
    int id=ba[0];

    //ɾ����������յ�beat�򲻴���
    if(idDict[id]==0)
        return;

    //����
    idDict[id]=1;

    //��ʾ�б�
    QString item=QString::number(id);
    item+=" ";
    item+=QString::fromLocal8Bit(ba.data()+1);
    item+=" "+targetAddr.toString();

    std::cout<<"receive a beat from "<<item.toLocal8Bit().data()<<std::endl;

    int size=ui->displayer->count();
    QString t;
    for(int i=0;i<size;i++){
        t=ui->displayer->item(i)->text();
        if(t==item)
            return;
        if(t.front()==item.front()){
            ui->displayer->item(i)->setText(item);
            return;
        }
    }
    QListWidgetItem tempItem;
    tempItem.setText(item);
    ui->displayer->addItem(item);


}

//��ʼ��
void MainWindow::init()
{
//    //���ý���
//    codec = QTextCodec::codecForName("utf8");

    //����ಥ��
    joinGroup();

    //rsa��ȡ��Կ��Կ
    getKeys();

    //����socket�󶨱���
    uniport=9997;
    uniSocket=new QUdpSocket();
    uniSocket->bind(QHostAddress::AnyIPv4, uniport);

    connect(uniSocket,  SIGNAL(readyRead()), this, SLOT(uniRecv()));

    //��ʱ����
    timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timedTask()));
    timer->start(2000);

}

//��ʱ���񣬼���û��Ƿ�����
void MainWindow::timedTask()
{
    std::cout<<"In timedTask()"<<std::endl;
    if(idDict.empty())
        return;
    for(int i=1; i<=idDict.size(); i++)
    {
        if(idDict[i]!=0)
            idDict[i]++;
    }

    QString t;
    int tempid;
    for(int i=0;i<ui->displayer->count();i++)
    {
        t=ui->displayer->item(i)->text().section(" ", 0, 0);
        tempid=t.toInt();
        std::cout<<tempid<<":"<<idDict[tempid]<<std::endl;
        if(idDict[tempid]>5)
        {
            int row=ui->displayer->row(ui->displayer->item(i));
            ui->displayer->takeItem(row);
            idDict[tempid]=0;
        }

    }
    timer->start(2000);
}

void MainWindow::on_deletePushButton_clicked()
{
    if(selectedItem==NULL)
        return;
    QString text=selectedItem->text();
    int id=text.section(" ", 0, 0).toInt();
    QString addr=text.section(" ", -1, -1);
    QHostAddress target(addr);
    std::cout<<id<<":"<<text.data()<<std::endl;

    uniSocket->writeDatagram(QByteArray("c"), target, targetPort);

    int row=ui->displayer->row(selectedItem);
    ui->displayer->takeItem(row);
    selectedItem=NULL;
    idDict[id]=0;
}

void MainWindow::on_displayer_itemClicked(QListWidgetItem *item)
{
    selectedItem=item;
}

