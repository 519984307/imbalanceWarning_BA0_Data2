#include "mydatetime.h"

myDateTime::myDateTime(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();
    getCurrentDateTime();
}


void myDateTime::getCurrentDateTime()
{
    emit sendCurrentDateTime(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QTimer::singleShot(1000,this,SLOT(getCurrentDateTime()));
}
