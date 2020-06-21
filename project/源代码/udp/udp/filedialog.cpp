#include "filedialog.h"
#include "ui_filedialog.h"

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);
    ui->playPushButton->setEnabled(false);
    player=new MediaPlayer(this);
}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::setFileName(QString f)
{
    fileName=f;
    ui->fileNameLabel->setText(fileName);
}

void FileDialog::setTotalSize(qint64 s)
{
    totalSize=s;
    QString text=QString::number(remainSize)+"/"+QString::number(totalSize);
    ui->hintLabel->setText(text);
    ui->progressBar->setMaximum(totalSize);
}

void FileDialog::setRemainSize(qint64 s)
{
    remainSize=s;
    QString text=QString::number(totalSize-remainSize)+"/"+QString::number(totalSize);
    ui->hintLabel->setText(text);
    ui->progressBar->setValue(totalSize-remainSize);
}

void FileDialog::setPlayEnabled(bool b)
{
    ui->playPushButton->setEnabled(b);
}

void FileDialog::display()
{
    this->show();
}

QString FileDialog::getFileName()
{
    return fileName;
}

void FileDialog::on_playPushButton_clicked()
{
    QString type=fileName.section(".", -1, -1);
    if(type=="mp3"||type=="MP3")
    {
        player->show();
        player->playMP3("./recv/"+fileName);
    }
    else if((type=="mp4"||type=="MP4"))
    {
        player->show();
        player->playMP4("./recv/"+fileName);
    }
}
