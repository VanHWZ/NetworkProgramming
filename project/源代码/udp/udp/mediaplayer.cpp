#include "mediaplayer.h"
#include "ui_mediaplayer.h"

MediaPlayer::MediaPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaPlayer)
{
    ui->setupUi(this);
    this->setWindowTitle("player");
    timer=new QTimer;
}

MediaPlayer::~MediaPlayer()
{
    delete ui;
}

void MediaPlayer::playMP3(QString path)
{
    player=new QMediaPlayer;
    player->setMedia(QMediaContent(QUrl::fromLocalFile(path)));
    duration=player->duration();
    ui->slider->setMaximum(duration);
    std::cout<<duration<<std::endl;
    ui->slider->setMinimum(0);
    ui->slider->setValue(position);
    ui->progress->setText(msecToStr(0)+"/"+msecToStr(duration));

    QString filename=path.section("/", -1, -1);
    this->setWindowTitle(filename);

    connect(player, &QMediaPlayer::positionChanged, [this](qint64 position){
            if(player->duration() != ui->slider->maximum())
            {
                ui->slider->setMaximum(player->duration());
                duration=player->duration();

            }
            ui->slider->setValue(player->position());

//            ui->slider->setValue(position);
    });
}

void MediaPlayer::playMP4(QString path)
{
    player=new QMediaPlayer;
    player->setMedia(QMediaContent(QUrl::fromLocalFile(path)));
    duration=player->duration();
    ui->slider->setMaximum(duration);
    std::cout<<duration<<std::endl;
    ui->slider->setMinimum(0);
    ui->slider->setValue(position);
    ui->progress->setText(msecToStr(0)+"/"+msecToStr(duration));

    QString filename=path.section("/", -1, -1);
    this->setWindowTitle(filename);

    QVBoxLayout layout;
    ui->widget->setLayout(&layout);

    videoWidget=new QVideoWidget;
    videoWidget->setMaximumSize(ui->widget->size());

    layout.addWidget(videoWidget);
    videoWidget->show();
    player->setVideoOutput(videoWidget);

    connect(player, &QMediaPlayer::positionChanged, [this](qint64 position){
            if(player->duration() != ui->slider->maximum())
            {
                ui->slider->setMaximum(player->duration());
                duration=player->duration();

            }
            ui->slider->setValue(player->position());

//            ui->slider->setValue(position);
    });
}

void MediaPlayer::moveSlider()
{
    int cur=ui->slider->value();
    cur=cur+1000 > duration ? duration : cur+1000;
    ui->slider->setValue(cur+1000);
    if(cur!=duration)
        timer->start(1000);
    else
    {
        ui->pushButton->setText("start");
        player->stop();
    }
}

void MediaPlayer::on_slider_valueChanged(int value)
{
    timer->stop();
    ui->progress->setText(msecToStr(value)+"/"+msecToStr(duration));
    timer->start(1000);
}

void MediaPlayer::on_pushButton_clicked()
{
    QMediaPlayer::State state=player->state();
    if(state==QMediaPlayer::PausedState||state==QMediaPlayer::StoppedState)
    {
        connect(timer, SIGNAL(timeout()), this, SLOT(moveSlider()));
        ui->pushButton->setText("stop");
        player->play();
        timer->start(1000);
    }else
    {
        ui->pushButton->setText("start");
        player->pause();
        timer->stop();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(moveSlider()));

//        std::cout<<duration<<std::endl;
    }
}

QString MediaPlayer::msecToStr(qint64 ms)
{
    int rawSec=ms/1000;
    int hour=rawSec/3600;
    int min=rawSec/60;
    int sec=rawSec%60;
    return hour==0?
                QString("%1").arg(min, 2, 10, QChar('0'))+":"+QString("%1").arg(sec, 2, 10, QChar('0'))
              :
                QString("%1").arg(min, 2, 10, QChar('0'))+":"+QString("%1").arg(min, 2, 10, QChar('0'))+":"+QString("%1").arg(sec, 2, 10, QChar('0'));
}

void MediaPlayer::on_slider_sliderReleased()
{
    if(timer->isActive())
        timer->stop();
    int value=ui->slider->value();
    ui->progress->setText(msecToStr(value)+"/"+msecToStr(duration));
    player->setPosition(value);
    timer->start(1000);
}

void MediaPlayer::closeEvent(QCloseEvent *event)
{
    player->stop();
    delete timer;
    delete player;
}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MediaPlayer m;
//    m.show();
////    m.playMP3("./recv/samsaramarigold.mp3");
//    m.playMP4("./recv/rnm.mp4");

//    return a.exec();
//}
