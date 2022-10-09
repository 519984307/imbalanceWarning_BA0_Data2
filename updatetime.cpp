#include "updatetime.h"
#include <QDebug>
#include <QTimer>
#include <QProcess>

updateTime::updateTime(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();
    updateTimeFunc();
}


void updateTime::updateTimeFunc()
{
    if(lastTime != currentTime){
        currentTime = lastTime;
        qDebug()<<"killDateStart";
        //kill掉原date -s进程
        QProcess psPro;
        psPro.start("ps");
        QString text  = "";
        QString delPID = "";
        if(!psPro.waitForFinished()){
            qDebug()<<"QProcess:"<<psPro.readAllStandardError();
        }else{
            text = psPro.readAllStandardOutput();//获取所有进程信息
            QStringList lineTextList = text.split("\n");//分割所有进程信息为行
            for(int i = 0;i<lineTextList.size();i++){
                QString lineText =  lineTextList.at(i);
                lineText = lineText.trimmed();
                if(lineText.contains("date -s")){
                    lineText.replace(QRegExp("[\\s]+"), " ");//多余空格变一个空格
                    QStringList oneList = lineText.split(" ");//用空格分割
                    delPID = oneList.at(0);//提取第一个即为PID
                    QString killBash = "kill -9 " + delPID;
                    qDebug()<<"kill date -s == "+ killBash +":";
                    if(delPID.size() > 0){
                        system(killBash.toLatin1().data());
                    }
                    break;
                }
            }
        }
        qDebug()<<"setDateStart";
        system((QString("date -s \"") +currentTime+QString("\" &")).toLatin1().data());
        //将系统时间写入RTC
        qDebug()<<"setHwclockStart";
        system("hwclock -w &");
        qDebug()<<"GetCurrentTime success "<<currentTime;
    }
    QTimer::singleShot(1000,this,SLOT(updateTimeFunc()));
}



void updateTime::slot_getDateTime(QString time)
{
    qDebug()<<"slot_getDateTime:"<<time;
    lastTime = time;
}
