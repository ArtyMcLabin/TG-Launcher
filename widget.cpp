//  (c)Arty McLabin
//   instruction:  "//+" indicates a "todo" symbol
//   instruction:  "//#" indicates a "fix/finish" symbol
//   instruction:  "//-" indicates a "to be removed" symbol

/* todo list:
 *
 * # check QMessageBox error on startWarband();
 * # resize back after exepath page
 * # add settings.value(skipVideo) bool to settings page
 *
 */


#include "widget.h"
#include "ui_widget.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QDesktopServices>
#include <QtDebug>
#include "tg_server.h"
#include "QVector"
#include <QPixmap>
#include <QPalette>
#include <QFile>
#include <QProgressBar>
#include <QSettings>
#include <QPoint>
#include <QProcess>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMessageBox>
#include <../QStringPaint/qstringpaint.h>
#include <psapi.h>






QSettings settings("TG-Launcher", "Arty McLabin");     //registry data object

QString htmlstr; //whole html data to be captured from the site and splited/translated later.
//  //  //  //
QString latestPatch = "unknown"; //yet
QString message = "no messages from admins.";
QString eventModule = "native";
QString eventTimeStart = "xx:xx";
QString eventTimeEnd = "yy:yy";
QString eventComment = "saturday event"; //+

QVector<int> port_vec;


QPoint mpos; //mouse position for some window dragging
////////////////////////////////////////////////////////headers + global vars end^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    init(); //# all kind of stuff




    tg_server siege("siege","siege",7240);
    tg_server battle("battle","battle",7250);
    tg_server duel("duel","duel",7252);
    tg_server arena("arena","arena",7244);
    port_vec.append(siege.port);
    port_vec.append(battle.port);
    port_vec.append(duel.port);
    port_vec.append(arena.port);





    sync_chain(); //starts sync chain
        //replyFinished(QNetworkReply*);
             //splitData();
                 //versionCheck();
                 //     //downloadPatch();
                 //          //bytesReadPatch(QNetworkReply*))
                 //               //unzip(file);
                 //                   //installPatch();
                 //# monitorSync();
                 //refreshUI();




}

Widget::~Widget()
{
    delete ui;
}

void Widget::init() //+
{
    qDebug() << "current dir is:" << QDir::currentPath();


    setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint ); //remove std windows border
    QObject::connect(ui->pushSite,SIGNAL(clicked()),this,SLOT(on_pushSite()));
    QObject::connect(ui->pushExit,SIGNAL(clicked()),this,SLOT(on_pushExit()));
    QObject::connect(ui->pushPlay,SIGNAL(clicked()),this,SLOT(on_pushPlay()));
    QObject::connect(ui->pushSettings,SIGNAL(clicked()),this,SLOT(on_pushSettings()));
    QObject::connect(ui->pushMinimize,SIGNAL(clicked()),this,SLOT(on_pushMinimize()));
    QObject::connect(ui->pushResetPatch,SIGNAL(clicked()),this,SLOT(on_pushResetPatch())); //-
    ui->progressBar->hide();
}

void Widget::unzip(QFile *archive)//#
{
    ui->labelPatch->setText(QStringPaint("Unzipping","#ea0003"));

  JlCompress::extractDir(archive->fileName(),"temp123");
  //+ escort&merge native folder inside(inside) temp123
  //+ ^ error handler


  if(archive->fileName()=="update.zip")
        installPatch();
//  if(archive->fileName()=="launcherUpdate.zip")
//        installLauncherUpdate();

  //+ delete *.zip

}

void Widget::installPatch() //#
{
    ui->labelPatch->setText(QStringPaint("Installing","#ea0003"));
    //+ install





    qDebug() << latestPatch << " patch installed successfuly!";
    settings.setValue("clientVersion", latestPatch);
    ui->labelPatch->setText("<span style=\" color:#0ab300;\">You have the last available TrollPatch: "+latestPatch+"</span>");
}


void Widget::on_pushExit()
{
    QApplication::exit();
}


void Widget::on_pushMinimize()
{
    QWidget::setWindowState(Qt::WindowMinimized);
    qDebug() << "minimized";
}

void Widget::on_pushResetPatch(){ //-
    settings.setValue("clientVersion", 0); //reset patch string
}

void Widget::on_pushReturn() //# split to 2 functions later
{
    ui->stackedWidget->setCurrentIndex(0);
    QObject::disconnect(ui->pushDetectPathToExe_return,SIGNAL(clicked()),this,SLOT(on_pushReturn()));
    QObject::disconnect(ui->pushSettings_return,SIGNAL(clicked()),this,SLOT(on_pushReturn()));
}

void Widget::on_pushSettings_acceptPath() //+
{
    QString path = ui->lineSettings_path->text();

         if(path.endsWith("mb_warband"))
        path += ".exe";
    else if(path.endsWith("\\")) //single backslash actually, wrote 2 because single \" destroys the quote symbol
        path += "mb_warband.exe";
    else if(!path.endsWith(".exe"))
        path += "\\mb_warband.exe";

    if(QFile::exists(path)){
        ui->lineSettings_path->setText(path);
        settings.setValue("executable",path);
        qDebug() << "executable path changed to: " << settings.value("executable");

        ui->labelSettings_pathMessage->setText("<span style=\" color:#00ff00;\">executable found & saved!</span>");
    }


    else{
        ui->labelSettings_pathMessage->setText("<span style=\" color:#ff0000;\">file doesn't exist</span>");
        settings.remove("executable");
        qDebug() << "executable path key deleted";
    }
}

void Widget::on_pushSettings_autodetectPath() //+
{

}

void Widget::replyFinished(QNetworkReply *pReply)
{
    QByteArray data=pReply->readAll();
    htmlstr = QString(data);
    //- qDebug() << endl << htmlstr;
    splitData(); //splitting the different infos from the html we've got (patch version, admin message, event etc.) to needed strings
}

void Widget::replyFinishedMonitor(QNetworkReply *pReply)
{
    QByteArray data=pReply->readAll();
    htmlstr = QString(data);
    qDebug() << "rf:" << htmlstr;
    splitDataMonitor();
}

void Widget::connectionError(QNetworkReply::NetworkError code)
{
    qDebug() << "error:" << code;
}

void Widget::splitData()
{
    htmlstr.remove(QRegExp("<[^>]*>"));   //clear <br> tags
      //annoying and ugly part that filters the right value from all the text..
    latestPatch = htmlstr.section(" ",1,1).section("\r",0,0);
    message = htmlstr.section("\"",1,1);
    eventTimeStart = htmlstr.section("saturdayEvent: ",1,1).section(" ",0,0);
    eventTimeEnd = htmlstr.section("- ",1,1).section(" ",0,0);
    eventModule = htmlstr.section("saturdayEvent: ",1,1).section(", ",1,1).section("\r",0,0);

    //- qDebug() << latestPatch << endl << message << endl << eventTimeStart << endl << eventTimeEnd << endl << eventModule;

    versionCheck();

    //# monitorSync(); //sync the different ports (tg servers) before full visual refresh
    refreshUI();
}

void Widget::splitDataMonitor()
{
    htmlstr.remove(QRegExp("<[^>]*>"));   //clear <br> tags
    qDebug() << "split data monitor:" << htmlstr;
}

void Widget::refreshUI()
{
    ui->labelMessage->setText("<html><head/><body><p align=\"center\"><span style=\" color:#f3870a;\">&quot;" + message + "&quot;</span></p></body></html>");
    ui->textBrowserEvent->setText("<span style=\" color:#b4b4b4;\"> Event Duration:   </span>" + eventTimeStart +"-"+ eventTimeEnd +"(pc time)<br><span style=\" color:#b4b4b4;\">Module/Mod:   </span>"+eventModule + "<br><span style=\" color:#b4b4b4;\">Comment:  </span>"+eventComment);
}

void Widget::sync_chain()
{
    QNetworkAccessManager* nam;

    nam = new QNetworkAccessManager(this);
          connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
          connect(nam, SIGNAL(error(QNetworkReply::NetworkError code)), this, SLOT(connectionError(QNetworkReply::NetworkError code)));

          nam->get(QNetworkRequest(QUrl("http://community.troll-game.org/launcherData/"))); //get whole HTML source code in a request
}

void Widget::monitorSync()
{
    for(int i=0; i<tg_server::serverCount;i++) //for each server
    {
    QNetworkAccessManager* nam;

    nam = new QNetworkAccessManager(this);
          connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinishedMonitor(QNetworkReply*)));
          QString stra = "http://troll-game.org:"+QString::number(port_vec.at(i))+"/";
          qDebug() << i << stra;
          QNetworkReply *reply = nam->get(QNetworkRequest(QUrl(stra)));
          connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(connectionError(QNetworkReply::NetworkError)));


    }

}

void Widget::on_pushSite()
{
    QDesktopServices::openUrl(QUrl("http://community.troll-game.org/"));
}

void Widget::on_pushSettings()
{
    ui->stackedWidget->setCurrentIndex(1);
    connect(ui->pushSettings_return,SIGNAL(clicked()),this,SLOT(on_pushReturn()));
    connect(ui->pushSettings_acceptPath,&QAbstractButton::clicked,this,&Widget::on_pushSettings_acceptPath);
    connect(ui->pushSettings_autodetectPath,&QAbstractButton::clicked,this,&Widget::on_pushSettings_autodetectPath);
    if(settings.contains("executable")){
    ui->lineSettings_path->setText(settings.value("executable").toString());
    ui->labelSettings_pathMessage->setText(QStringPaint("Executable found",Qt::green));

    }
}

void Widget::on_pushPlay() //#
{
#ifdef Q_OS_WIN32

    if(!settings.contains("executable"))
              detect_PathToExe();
    else{
      int error = startWarband();   //start warband and save errorcode

                         switch(error)
                         {
                             case 1:
                                                {
                                                 qDebug() << "unable to start warband, wrong path";
                                                 detect_PathToExe();
                                                 ui->labelDetectPathToExe->setText("error ocured finding the executable in the path, did you move it?\n launch warband again for me to find it \nor change it manually in the settings");
                                                 break;
                                                }
                             case 2:
                                             {
                                              qDebug() << "unable to start warband, wrong handle";
                                              QMessageBox* errorWindow = new QMessageBox();
                                              errorWindow->setText("error2: handle not found in startWarband()\nplease report to jesus in tg forums.");
                                              errorWindow->show();
                                                 break;
                                             }
                             case 3:
                                            {
                                            QMessageBox* errorWindow = new QMessageBox();
                                            errorWindow->setText("error3: handle not found in startWarband()video\nplease report to jesus in tg forums.");
                                            errorWindow->show();
                                                 break;
                                            }
                             case 0:
                                 qDebug()<<"warband started without error";
                                 switchToPlayMode();
                                 break;
                          }
         }
#endif
}

void Widget::versionCheck()
{
    if(!settings.contains("clientVersion") || settings.value("clientVersion")==0){
        qDebug() << "starting initialization chain";
        ui->labelPatch->setText("<span style=\" color:#ea0003;\">Initialization Update Needed: "+latestPatch+"</span>");
        ui->pushUpdate->setEnabled(1);
        settings.setValue("clientVersion", 0); //creating registry key
        QObject::connect(ui->pushUpdate,SIGNAL(clicked()),this,SLOT(downloadPatch()));
      }

    else if(settings.value("clientVersion") != latestPatch){
   ui->labelPatch->setText("<span style=\" color:#ea0003;\">New available TrollPatch! : "+latestPatch+"</span>");
   ui->pushUpdate->setEnabled(1);
   QObject::connect(ui->pushUpdate,SIGNAL(clicked()),this,SLOT(downloadPatch()));
    }
    else
        ui->labelPatch->setText("<span style=\" color:#0ab300;\">You have the latest available TrollPatch: "+latestPatch+"</span>");
}

void Widget::downloadPatch()    //+ftp url sync
{
 ui->labelPatch->setText("<span style=\" color:#ea0003;\">      Downloading      </span>");

    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

       QNetworkReply *reply = nam->get(QNetworkRequest(QUrl("http://community.troll-game.org/index.php?/files/getdownload/263-trollgame-patch/")));
          connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(bytesReadPatch(QNetworkReply*)));
          connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(connectionError(QNetworkReply::NetworkError)));
          connect(reply, SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(updateDownloadProgress(qint64,qint64)));
}

void Widget::updateDownloadProgress(qint64 bytes, qint64 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(bytes);
    ui->pushUpdate->hide();
    ui->progressBar->show();

    qDebug() << bytes << "from" << total;
}

void Widget::bytesReadPatch(QNetworkReply *pReply)
{
    qDebug() << "bytesReadPatch()";
    QFile *file = new QFile("update.zip");
    if (!file->open(QIODevice::WriteOnly))
        qDebug() << "file doen't exit";
    else{
    file->write(pReply->readAll());
    qDebug() << "written some data";
    unzip(file);
    }

              file->close();
}


void Widget::detect_PathToExe()
{
ui->stackedWidget->setCurrentIndex(2);
this->resize(5,5);
  connect(ui->pushDetectPathToExe_return,SIGNAL(clicked()),this,SLOT(on_pushReturn()));
//detach waitForExe() to other thread to avoid GUI freeze while waiting for warband handle
  QFuture<bool> future = QtConcurrent::run(this,waitForExe);

  if(future.result() == 0) //return 0 all's k
          on_pushPlay();


}

bool Widget::waitForExe() //#
{
#ifdef Q_OS_WIN32

    if(settings.contains("executable"))
        ui->labelPathToExe->setText(QStringPaint(settings.value("executable").toString(),Qt::red));

         HWND hWnd;
         do{
          hWnd = ::FindWindow(NULL, L"Mount&Blade Warband");
         }while(!hWnd);                                                    // will loop a goto FindWindow() till hWnd captured (window found)

      qDebug() << "warband window found in waitForExe()";
      DWORD PID = 0x0;
      GetWindowThreadProcessId( hWnd, &PID );
      qDebug() << "PID: " << PID;


      HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
      if(handle == 0){
          qDebug()<<"no handle after openProcess() :(";
          return 1;
      }
      else{
          wchar_t exe_path[2048] = {};

          if (GetModuleFileNameEx(handle, 0, exe_path, sizeof(exe_path)-1)){
            settings.setValue("executable",QString::fromWCharArray(exe_path));
             qDebug() << "exe_path" << QString::fromWCharArray(exe_path) << "saved to registry" << "waiting for handle close, make sure!!!!";


                              if(CloseHandle(handle)){
                              qDebug() << "handle closed, all's k.";
                              return 0;
                              } else return 1;
          }

          else{
            qDebug() << "GetModuleFileNameEx() failed: " << GetLastError();
            return 1;
          }
      }



#endif
}

int Widget::startWarband()
{
#ifdef Q_OS_WIN32
    if(!QFile::exists(settings.value("executable").toString())){
        qDebug() << "startWarband() error1: executable doesn't exist!";
        return 1;
    }

    QProcess* warband_proc = new QProcess(this);
    warband_proc->setProgram(settings.value("executable").toString());
    warband_proc->start();

        int i=0;
        HWND hWnd;
    do{
    QThread::msleep(50);
    hWnd = ::FindWindow(NULL, L"Mount&Blade Warband");
    }while(!hWnd && ++i<100);
    if(i>=100){ qDebug()<<"startWarband() error2: menu window handle not found";    return 2; }

        ::SetForegroundWindow(hWnd);
        keybd_event(VK_RETURN, 0x0D,0,0);                      //send Enter key into native warband launcher
        keybd_event(VK_RETURN, 0x0D,KEYEVENTF_KEYUP,0);

if(settings.value("skipVideo").toBool())
    if(skipVideo()==1) //returns 1(aka)error3 or 0(all'sk), see skipVideo()
        return 3;

//if() do foo();
//if() do bar();
        return 0;
#endif
}

bool Widget::skipVideo()
{
#ifdef Q_OS_WIN32

    int i=0;
    HWND hWnd=NULL;
do{
QThread::msleep(50);
hWnd = ::FindWindow(NULL, L"Bink Video Player");
}while(!hWnd && ++i<150);
if(i>=150){ qDebug()<<"startWarband() error3: video window handle not found";    return 1; }

::SetForegroundWindow(hWnd);
keybd_event(VK_ESCAPE, 0x1B,0,0);                      //skip video
keybd_event(VK_ESCAPE, 0x1B,KEYEVENTF_KEYUP,0);
return 0;

#endif
}

void Widget::switchToPlayMode() //+
{
 ui->stackedWidget->setCurrentIndex(3);
}


void Widget::mousePressEvent(QMouseEvent *event){
    mpos = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() && Qt::LeftButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}
