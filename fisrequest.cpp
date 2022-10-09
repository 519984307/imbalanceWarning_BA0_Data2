#include "fisrequest.h"
#include <QApplication>
#include <QTime>
#include <QDebug>

fisRequest::fisRequest(QObject *parent) :
    QObject(parent)
{
    this->moveToThread(&thread);
    thread.start();

    query = new sqlQuery;
    connect(this,SIGNAL(sendRequestResult(QVariantMap)),query,SLOT(insertProcess(QVariantMap)));
//    query->selectAllCarType();//请求接口前先刷新当前数据库中的车型信息

//    connect(this,SIGNAL(sendSelectFisData(QString)),query,SLOT(sqlExec(QString)));

    updtTime = new updateTime;
    connect(this,SIGNAL(sendDateTime(QString)),updtTime,SLOT(slot_getDateTime(QString)));

    pinCodeEnd = 1000000;
}

fisRequest::~fisRequest()
{
    query->deleteLater();
    updtTime->deleteLater();
}

void fisRequest::fisRequestIni()
{
    conf = new QSettings("/config.ini", QSettings::IniFormat);
    DataServerIp = conf->value("baseinfo/DataServerIp").toString();
    ServerPort = conf->value("baseinfo/ServerPort").toString();
//    requestID = conf->value("baseinfo/requestID").toInt();
    manager = new QNetworkAccessManager(this);
    requestRun();
}


void fisRequest::requestRun()
{
    QVariantMap dataMap;
    QDateTime time1 = QDateTime::fromString("00:00","mm:ss");
    QDateTime time2 = QDateTime::fromString(QDateTime::currentDateTime().toString("mm:ss"),"mm:ss");
    int basic = time1.secsTo(time2);
    if(0 == basic%(10*60)){//10分钟更新一次时间
        url = "/submit";//请求url，暂未知
        dataMap.insert("Type","Get");
        dataMap.insert("Target","GetCurrentTime");
        httpRequest(url,dataMap);
    }else if(0 == basic%(10*60)){//10分钟更新一次时间

    }else if(0 == basic%(10*60)){//10分钟更新一次时间

    }else if(0 == basic%(10*60)){//10分钟更新一次时间

    }else if(0 == basic%17){//17秒请求一次接口fis
        url = "/submit";//请求url，暂未知
        dataMap.insert("Type","Get");
        dataMap.insert("Target","FisPreview");//请求内容，暂未知
        QVariantMap ParameterMap;
        ParameterMap.insert("Line_ID",-1);//-1:进站出站的数据都要
        ParameterMap.insert("ID",conf->value("baseinfo/requestID").toInt());
        dataMap.insert("Parameter",ParameterMap);
        httpRequest(url,dataMap);
    }
    QTimer::singleShot(1000,this,SLOT(requestRun()));
}


void fisRequest::httpRequest(QString url, QVariantMap dataMap)
{
    QJson::Serializer serializer;
    bool ok;
    QByteArray json = serializer.serialize(dataMap, &ok);
    req.setUrl(QUrl("http://"+DataServerIp+":"+ServerPort+url));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader, json.length());
    QVariantMap result;
    bool queryResult = false;
    if(ok){
        qDebug()<<"reply:"<<req.url()<<json;
        QNetworkReply *reply = manager->post(req,json);
        reply->ignoreSslErrors();
        QTime t_time;
        t_time.start();

        bool timeout = false;
        int timeCountTemp = 0;
        while (!reply->isFinished()) {
            int currentTime = t_time.elapsed();
            if(currentTime%15 == 0 && timeCountTemp != currentTime){
                timeCountTemp = currentTime;
                QApplication::processEvents();
            }
            if (currentTime >= 5*1000) {
                timeout = true;
                qDebug()<< "fisRequest reply timeout fail:"<<url;
                break;
            }
        }
        if(!timeout && reply->error() == QNetworkReply::NoError){
            QByteArray bytes = reply->readAll();
//            qDebug()<<"*fisRequest post receive*"<<url<<bytes;//打印所有信息
            QJson::Parser parser;
            bool ok;
            result = parser.parse(bytes, &ok).toMap();
            if(!ok){
//                qDebug()<<"fisRequest An error occurred during parsing"<<bytes;
            }else{
                if(url == "/submit" && result["Target"].toString() == "FisPreview"){//请求车型的接口
                    //收到回复的"Result"字段为1，数据正常，可以提取
                    QVariantList valList = result["Value"].toList();
                    int length = valList.size();
                    qDebug()<<"reValue.size():"<<length;
                    if(length > 0){
                        queryResult = true;
                        for(int i=0;i<length;i++){
                            emit sendRequestResult(valList[i].toMap());//循环将请求的每一条值发出去存储
                        }
                    }
                }else if(url == "/submit" && result["Target"].toString() == "GetCurrentTime"){//同步时间的接口
                    emit sendDateTime(result["CurrentTime"].toString());
                }

                if(result["Result"].toBool()){
                    qDebug()<<"fisRequest:[\"Result\"]SUCESS:"<<result["Result"]<<url;
                }else{
                    qDebug()<<"fisRequest:[\"Result\"]FALSE:"<<result["Result"]<<url;
                }

            }
        }else if(reply->error() != QNetworkReply::NoError){
            QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
            qDebug()<<"fisRequest found error:"<<statusCodeV.toInt()<<reply->error()<<reply->errorString();
            reply->deleteLater();
        }else{
            qDebug() << "fisRequest serializer.serialize error:"<<serializer.errorMessage();
        }
    }

    if(!queryResult){//没有请求到一样要去触发逻辑来做界面显示，主要是开机时必需要有
        QVariantMap map;
        emit sendRequestResult(map);
    }
//    conf->setValue("baseinfo/requestID",requestID);

#if 0 //手动生成数据测试
    myRandCarInfo();
#endif

}


void fisRequest::myRandCarInfo()
{
//    qDebug()<<"rand():"<<rand()<<rand();
//    QVariantMap result;
//    int line_id = 1;
//    if(rand()%2 == 0){
//        line_id = 2;
//    }
//    result.insert("LineID",line_id);
//    QStringList carList_l1;
//    QStringList carList_l2;
//    carList_l1<<"AL"<<"NF"<<"TP"<<"YK"<<"ZJ";
//    carList_l2<<"PU"<<"NF"<<"TP"<<"DV";
//    QString carType;
//    if(line_id == 1){
//        int index = (rand()%100+rand()%100)%carList_l1.size();
//        carType = carList_l1[index];
//    }else if(line_id == 2){
//        int index = (rand()%100+rand()%100)%carList_l2.size();
//        carType = carList_l2[index];
//    }
//    result.insert("carType",carType);
//    result.insert("InTime","2021-03-04 07:10:30");
//    QString status;
//    QString vinCode = "LSVAE45E8F";
//    QString outTime = "2021-03-10 13:11:12";

//    int val_M = 13;
//    int val_N = 8;
//    if(pinCodeEnd-1000000 > 1000){
//        val_N = 4;
//    }else if(pinCodeEnd-1000000 > 2000){
//        val_N = 6;
//    }
//    if(rand()%val_M < val_N){
//        outTime = "";
//        status = "IN";
//        vinCode.append(QString::number(pinCodeEnd++));
//    }else{
//        if(pinCodeEnd-1000000 < 11){
//            outTime = "";
//            status = "IN";
//            vinCode.append(QString::number(pinCodeEnd++));
//        }else{
//            status = "OUT";
//            vinCode.append(QString::number(pinCodeEnd-((rand()%100000+rand()%1000000)%(pinCodeEnd-1000000))));
//        }
//    }
//    result.insert("VINCode",vinCode);
//    result.insert("OutTime",outTime);
//    result.insert("Status",status);
//    emit sendRequestResult(result);//格式已更改，不再用
}
