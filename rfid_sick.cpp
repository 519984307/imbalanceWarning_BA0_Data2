#include "rfid_sick.h"
#include <QDebug>

rfid_SICK::rfid_SICK(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();

    conf = new QSettings("/config.ini", QSettings::IniFormat);
    connectRfid();
}

rfid_SICK::~rfid_SICK()
{
    conf->deleteLater();
}

void rfid_SICK::connectRfid()
{
    socket = new QTcpSocket;
    connect(socket,SIGNAL(readyRead()),this,SLOT(receiversRfid()));
    QString rfidip = conf->value("baseinfo/RfidIp").toString();
    int rfidport  = conf->value("baseinfo/RfidPort").toInt();
    socket->connectToHost(rfidip,rfidport);
    socket->waitForConnected(1000);
    qDebug()<<"rfid_SICK waitForConnected:"<<socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState){
        qDebug()<<"rfid_SICK connect success";
    }else{
        qDebug()<<"rfid_SICK connect fail";
        QTimer::singleShot(5*1000,this,SLOT(connectRfid()));
    }
}

void rfid_SICK::receiversRfid()
{
    QByteArray data = socket->readAll();
    data.replace(" ", "");//去掉所有空格
    qDebug()<<"receiversRfid:"<<data.size()<<data;
    if(readData != data){//rfid会一直扫一直发，所以需要检查重复项
        readData = data;
        qDebug()<<"read rfid new data:";
        if(readData.size() == 60){
//            qDebug()<<"read rfid data is normal!";
//            QString asciiStr = myLongStringToAscii(readData);
            QString asciiStr = QByteArray::fromHex(readData);
            QString pinStr = asciiStr.mid(0,29);//测试取的PIN码，项目计划取的是线号和PIN码
            qDebug()<<"read pin:"<<pinStr;
            emit sendRFIDPINCode(pinStr);
        }
    }
}

//form:365943363230313736333033373236343453373325583958393131313121
//to:6YC62017630372644S73%X9X91111!
//QByteArray::fromHex(QByteArray::data)
QString rfid_SICK::myLongStringToAscii(QString longStr)
{
    QString asciiStr;
    for(int i=0;i<longStr.size()/2;i++){
        char ch = longStr.mid(i*2,2).toInt(0,16);
        QByteArray buf;
        buf[0] = ch;
        asciiStr.append(buf);
    }
    return asciiStr;
}
