#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>
#include <QUdpSocket>
#include <QFile>
#include <string>
#include "rsa.h"
#include "mediaplayer.h"

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = nullptr);
    ~FileDialog();
    QString getFileName();

public slots:
    void setFileName(QString);
    void setTotalSize(qint64);
    void setRemainSize(qint64);
    void setPlayEnabled(bool);
    void display();

private slots:
    void on_playPushButton_clicked();

private:
    Ui::FileDialog *ui;
    QString fileName;
    qint64 totalSize;
    qint64 remainSize=0;

    MediaPlayer*player;
};

#endif // FILEDIALOG_H
