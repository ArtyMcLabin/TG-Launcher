#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QString>
#include <QMouseEvent>
#include <JlCompress.h>

#ifdef Q_OS_WIN32
  #include "qt_windows.h"   // manipulate external process on windows (warband client)
#endif


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void init();
    void unzip(QFile* archive);
    void installPatch();

    int startWarband();
    bool skipVideo();
    void switchToPlayMode();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    Ui::Widget *ui;

public slots:
    void on_pushExit();
    void on_pushMinimize();
    void on_pushSite();
    void on_pushSettings();
    void on_pushPlay();
    void on_pushResetPatch(); //-
    void on_pushReturn();
    void on_pushSettings_acceptPath();
    void on_pushSettings_autodetectPath();

    void replyFinished(QNetworkReply* pReply);
    void replyFinishedMonitor(QNetworkReply* pReply);
    void connectionError(QNetworkReply::NetworkError code);
    void splitData();
    void splitDataMonitor();
    void refreshUI();
    void sync_chain();
    void monitorSync();
    void versionCheck();

    void downloadPatch();
    void updateDownloadProgress(qint64 bytes, qint64 total);
    void bytesReadPatch(QNetworkReply* pReply);

    void detect_PathToExe();
    bool waitForExe();

};

#endif // WIDGET_H
