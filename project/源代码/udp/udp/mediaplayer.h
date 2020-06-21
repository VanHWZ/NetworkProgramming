#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QTimer>
#include <QCloseEvent>
#include <iostream>

namespace Ui {
class MediaPlayer;
}

class MediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MediaPlayer(QWidget *parent = nullptr);
    ~MediaPlayer();

    void playMP3(QString);
    void playMP4(QString);
    QString msecToStr(qint64 ms);

protected:
    void closeEvent(QCloseEvent*event);

public slots:
    void moveSlider();

private slots:
    void on_slider_valueChanged(int value);

    void on_pushButton_clicked();

    void on_slider_sliderReleased();

private:
    Ui::MediaPlayer *ui;
    qint64 duration, position;
    QVideoWidget*videoWidget;
    QMediaPlayer*player;
    QTimer*timer;

};

#endif // MEDIAPLAYER_H
