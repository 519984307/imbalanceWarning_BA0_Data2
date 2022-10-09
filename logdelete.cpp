#include "logdelete.h"
#include <QDebug>

logDelete::logDelete(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();
    timer = new QTimer;
}

logDelete::~logDelete()
{
    timer->deleteLater();
}

void logDelete::logDelIni()
{
    connect(timer,SIGNAL(timeout()),this,SLOT(logDelFunc()));
    timer->start(1*60*60*1000);//1小时检测一次
}


void logDelete::logDelFunc()
{
    QDir dir("/log");
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    do{
        QFileInfo fileInfo = list.at(i);
        QString fileName = fileInfo.fileName();//文件名（带后缀）
        if(fileName=="."||fileName==".."){
            i++;
            continue;
        }
        QString baseName = fileInfo.baseName();//文件名（不带后缀）
        if(QDateTime::fromString(baseName.left(10),"yyyy-MM-dd").daysTo(QDateTime::currentDateTime()) > 10){
            qDebug() << "log delete file "+fileInfo.fileName();
            QFile(fileInfo.filePath()).remove();
        }
        i++;
    }while(i<list.size());
}
