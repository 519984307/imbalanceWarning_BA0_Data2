#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "./TestInput_Cursor/inputmethod.h"
#include "fisrequest.h"
#include "sqlquery.h"
#include "mydatetime.h"
#include "logdelete.h"
#include <QDebug>
#include "rfidmanage.h"
#include "rfid_sick.h"

/******************日志打印级别函数****************************/
void outputMessage(QtMsgType type, const char *msg)
{
    QDir dir("/log");
    if (!dir.exists()){
        dir.mkdir("/log");
    }

    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
        abort();
    }
    if(text != QString("Warning:"))
    {
        QString message = QString("[%1] %2 %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz ddd")).arg(text).arg(msg);
        QDateTime time = QDateTime::currentDateTime();
        QString date = time.toString("yyyy-MM-dd_hh");

        QFile file(QString("/log/")+date+QString(".log"));
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text_stream(&file);
        text_stream << message << endl;
        file.flush();
        file.close();
    }
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    qRegisterMetaType<QList<QPair<QString,QVariantMap> > >("QList<QPair<QString,QVariantMap> >");
    qRegisterMetaType<QList<QVariantMap> >("QList<QVariantMap>");
    //显示中文
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(utf8);
    QTextCodec::setCodecForLocale(utf8);
    QTextCodec::setCodecForCStrings(utf8);

    qInstallMsgHandler(outputMessage);//重定向日志到文件

#if 1
    QApplication::setOverrideCursor(Qt::BlankCursor);//去掉光标
#endif

    //没requestID配置就配置一个,给个默认值
    if(!QSettings("/config.ini", QSettings::IniFormat).contains("baseinfo/requestID")){
        QSettings("/config.ini", QSettings::IniFormat).setValue("baseinfo/requestID",-1);//有没有值开机都从-1开始
    }

    qDebug()<<"main() Version:V.Dat2.21.11.22.1";

    //调用键盘类
    QInputContext *im = new InputMethod;
    a.setInputContext(im);

    fisRequest *fisReq = new fisRequest;
    QTimer::singleShot(5,fisReq,SLOT(fisRequestIni()));

//    sqlQuery * query = new sqlQuery;
    MainWindow w;
//    QObject::connect(w.queryDia,SIGNAL(sendQueryStr(QString)),query,SLOT(sqlExec(QString)));
//    QObject::connect(query,SIGNAL(sendSqlModelData(QSqlQuery*)),w.queryDia,SLOT(slot_getQueryModelIniData(QSqlQuery*)));

    QObject::connect(fisReq->query,SIGNAL(sendCurrentCarTypeList(QList<QVariantMap>)),&w,SLOT(slot_currentCarTypeList(QList<QVariantMap>)));
//    QObject::connect(&w,SIGNAL(sendSelectAllCarType()),fisReq->query,SLOT(selectAllCarType()));
//    fisReq->query->selectAllCarType();

    myDateTime * myTime = new myDateTime;
    QObject::connect(myTime,SIGNAL(sendCurrentDateTime(QString)),&w,SLOT(slot_getDateTime(QString)));

    w.show();

    logDelete * logDel = new logDelete;
    QTimer::singleShot(1000,logDel,SLOT(logDelIni()));

#if 1
    rfid_SICK* sick = new rfid_SICK;
    QObject::connect(sick,SIGNAL(sendRFIDPINCode(QString)),fisReq->query,SLOT(getRFIDCodeList(QString)));
#endif

    fisReq->query->selectAllCarType();//请求接口前先刷新当前数据库中的车型信息
    return a.exec();
}
