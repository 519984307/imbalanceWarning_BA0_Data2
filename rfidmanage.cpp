#include "rfidmanage.h"
#include <QFile>
#include <QDebug>

rfidManage::rfidManage(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();

    timer1 = new QTimer;
    connect(timer1,SIGNAL(timeout()),this,SLOT(readRfid()));
    conf = new QSettings("/config.ini", QSettings::IniFormat);
}

rfidManage::~rfidManage()
{
    timer1->deleteLater();
    conf->deleteLater();
}

void rfidManage::rfidInit()
{
#if 1
    connectRfid();
#else
    if(getRfidConnected()){
        connectRfid();
    }else{
        QTimer::singleShot(1000,this,SLOT(rfidInit()));
    }
#endif
}

void rfidManage::connectRfid()
{
    socket = new QTcpSocket;
    connect(socket,SIGNAL(readyRead()),this,SLOT(receiversRfid()));
    QString rfidip = conf->value("baseinfo/RfidIp").toString();
    int rfidport  = conf->value("baseinfo/RfidPort").toInt();
    socket->connectToHost(rfidip,rfidport);
    socket->waitForConnected(1000);
    qDebug()<<"rfid waitForConnected:"<<socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState){
        qDebug()<<"rfid connect success";
        writeRfid("reset");
    }else{
        qDebug()<<"rfid connect fail";
    }
}

void rfidManage::disConnectRfid()
{
    timer1->stop();
    socket->deleteLater();
    connectRfid();
}

void rfidManage::displayError(QAbstractSocket::SocketError)
{
    qDebug() << "rfid displayError:" <<socket->errorString(); //输出错误信息
    disConnectRfid();
}

void rfidManage::receiversRfid()
{
    QString data = socket->readAll();
    qDebug()<<"receiversRfid:"<<data;
    QDomDocument doc;
    doc.setContent(data);
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    while(node.childNodes().size() > 0){
        QDomElement element = node.toElement();
        QString tagName = element.tagName();
        if(tagName == "resultCode"){
            QString resultCode = element.text();
            qDebug()<<"rfid resultCode:"<<resultCode;
            if(resultCode != "0000"){//readwrite fail
                if(resultCode == "0018" || resultCode == "0019" || resultCode == "001F"){
                    qDebug()<<"this is reset rfid";
                    writeRfid("reset");
                }else{
                    qDebug()<<"RFID read error data";
                    disConnectRfid();
                }
            }
        }else if(tagName == "reset"){
            qDebug()<<"rfid reset success";
            timer1->start(500);
        }else if(tagName == "writeTagData"){
            qDebug()<<"rfid writeTagData success";
        }else if(tagName == "readTagData"){
            parseReadTagData(element);
        }
        node = node.nextSibling();
    }
}

void rfidManage::parseReadTagData(QDomElement element)
{
    QDomNode node = element.firstChild();
    while(node.childNodes().size() > 0){
        QDomElement element = node.toElement();
        QString tagName = element.tagName();
        if(tagName == "returnValue"){
            QDomNode node = element.firstChild();
            while(node.childNodes().size() > 0){
                QDomElement element = node.toElement();
                QString tagName = element.tagName();
                if(tagName == "data"){
                    qDebug()<<"readData:"<<element.text();
                }
                node = node.nextSibling();
            }
        }
        node = node.nextSibling();
    }
}

int rfidManage::getRfidConnected()
{
    QFile wifi_test("/var/test_wifi");
    int status = 0;
    if(wifi_test.open(QIODevice::ReadOnly)){
        QByteArray test_wifiBuf = wifi_test.readAll();
//        gateway = test_wifiBuf.mid(0,1).toInt();
//        dataserver = test_wifiBuf.mid(2,1).toInt();
        status = test_wifiBuf.mid(4,1).toInt();
        wifi_test.close();
    }
    return status;
}

void rfidManage::readRfid()
{
    writeRfid("readTagData");
}

void rfidManage::writeRfid(QString val)
{
    QString head = "<command><writeTagData><startAddress>0000</startAddress><data>";
    QString tail = "0D</data></writeTagData></command>";//0x0D->回车键
    QString writeStr = head + val + tail;
    if(val == "reset"){
        writeStr = "<command><reset><param>0005000000010001</param></reset></command>";
    }else if(val == "readTagData"){
        writeStr = "<command><readTagData><startAddress>0000</startAddress><dataLength>0015</dataLength></readTagData></command>";
    }
    socket->write(writeStr.toLatin1());
}
