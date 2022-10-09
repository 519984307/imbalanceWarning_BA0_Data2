#include "sqlquery.h"
#include <QDebug>
#include <QDateTime>

sqlQuery::sqlQuery(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    this->moveToThread(thread);
    thread->start();
    conf = new QSettings("/config.ini", QSettings::IniFormat);
    sqlConnect();
    if(sqlOpen()){
        query->exec("CREATE TABLE IF NOT EXISTS `carInfo` ("
                    "`ID` int(11) NOT NULL AUTO_INCREMENT,"
                    "`LineID` int(11) DEFAULT NULL,"
                    "`PINCode` varchar(200) DEFAULT NULL,"
                    "`G9` varchar(50) DEFAULT NULL,"
                    "`carType` varchar(50) DEFAULT NULL,"
                    "`InTime` datetime DEFAULT NULL,"
                    "`OutTime` datetime DEFAULT NULL,"
                    "`Status` varchar(50) DEFAULT NULL,"
                    "PRIMARY KEY (`ID`)"
                  ") ENGINE=InnoDB AUTO_INCREMENT=111 DEFAULT CHARSET=latin1;");
    }
    sqlClose();
}
sqlQuery::~sqlQuery()
{
    conf->deleteLater();
//    qDebug()<<"sqlQuery close";
}

void sqlQuery::sqlConnect()
{
//    qDebug()<<QSqlDatabase::connectionNames();
//    qDebug()<<"QSqlDatabase::contains:"<<QSqlDatabase::contains("qt_sql_default_connection")
//           <<QSqlDatabase::contains("carInfoSqlConnection");
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        sqlDB = QSqlDatabase::database("qt_sql_default_connection");
    }else{
        if(!QSqlDatabase::contains("carInfoSqlConnection")){
            sqlDB = QSqlDatabase::addDatabase("QMYSQL","carInfoSqlConnection");
        }else{
            sqlDB = QSqlDatabase::addDatabase("QMYSQL");
        }
    }
//    qDebug()<<"connectionNames:"<<sqlDB.connectionNames();//第一次连接是("carInfoSqlConnection") ，后面都是("carInfoSqlConnection", "qt_sql_default_connection")
    sqlDB.setHostName("localhost");
    sqlDB.setDatabaseName("Tighten");
    sqlDB.setUserName("root");
    sqlDB.setPassword("123456");
    query = new QSqlQuery(sqlDB);
}

bool sqlQuery::sqlOpen()
{
    if(!sqlDB.isOpen() || !sqlDB.contains("carInfoSqlConnection")){
        if(sqlDB.open()){
//            qDebug()<<"open carInfoSqlConnection success";
        }else{
            qDebug()<<"open carInfoSqlConnection faile:"<<sqlDB.lastError().text();
        }
    }

    if(sqlDB.isOpen() && sqlDB.contains("carInfoSqlConnection")){
        return true;
    }else{
        return false;
    }
}

void sqlQuery::sqlClose()
{
    if(sqlDB.isOpen()){
        sqlDB.close();
    }
}
void sqlQuery::sqlExec(QString queryStr)
{
    if(sqlOpen()){
        query->exec(queryStr);
        qDebug()<<"sqlExec:"<<queryStr;//<<query->executedQuery();
    }
    sqlClose();
    emit sendSqlModelData(query);
}


QString sqlQuery::myQueryFromBoundValues(QString query, QVariantMap map)
{
    QStringList keyList = map.keys();
    for(int i=0;i<keyList.size();i++){
        QString key = keyList[i];
        query.replace(key,"'" + map.value(key).toString() + "'");
    }
    return query;
}


//接收接口请求的车型一条数据，存储到本地数据库
void sqlQuery::insertProcess(QVariantMap map)
{
    if(map.size() > 0){
        //    收到请求的数据再看RFID列表里有没有，有就一起存储算一次有效数据，更新requestID/RFID列表；
        //    如果RFID列表没有就先存储请求到的数据，也更新requestID，等RFID扫描后再补充线号后更新RFID列表。
        int IDMin = 0;
        int IDMax = 0;
        int count = 0;
        int RecordID = map.value("RecordID").toInt();
        int Line_ID = map.value("Line_ID").toInt();//接收接口的字段 0：进站  1：1线出站  2：2线出站
        int LineID = 0;//存储本地的字段 0：无效  1：1线  2：2线
        QString pinCode = map["Werk"].toString() + map["SPJ"].toString() + map["KNR"].toString();
        QString G9 = "";
        QString carType = map.value("CarModel").toString().trimmed();
        QString carVIN = map["VIN"].toString();
        QString Status = "";
        if(Line_ID == 0){
            Status = "IN";
        }else{
            Status = "OUT";
        }
        if(carVIN.mid(6,2) == "3E"){//C级车直接归入2线 G9默认值为DS111
            LineID = 2;
            G9 = "DS111";
        }else{//其他车按RFID读取的线号来归纳
            if(rfidList.contains(pinCode)){
                int index = rfidList.indexOf(pinCode); // 当前pin码在列表中哪个坐标
                LineID = lineIDList[index].toInt();
                G9 = G9List[index];
            }
        }
        if(sqlOpen()){
            //        if(Line_ID > 0){
            if(Status == "IN"){//进站小车存储过程
                query->exec("SELECT MIN(ID) FROM carInfo WHERE `Status` = 'OUT';");
                query->next();
                IDMin = query->value(0).toInt();//只替换最早并且已出站的数据

                query->exec("SELECT MAX(ID),COUNT(*) FROM carInfo;");
                query->next();
                IDMax = query->value(0).toInt();
                count = query->value(1).toInt();
                qDebug()<<"min max count:"<<IDMin<<IDMax<<count;

                if(count < 3000){
                    query->prepare("INSERT INTO `Tighten`.`carInfo` (`ID`, `LineID`, `PINCode`, `G9` , `carType`, `InTime`, `OutTime`, `Status`) "
                                   "VALUES (:ID, :LineID, :PINCode, :G9 ,:carType, :InTime, :OutTime, :Status);");
                }else{
                    query->prepare("UPDATE `Tighten`.`carInfo` SET `ID`=:ID, `LineID`=:LineID, `PINCode`=:PINCode, `G9`=:G9, `carType`=:carType, "
                                   "`InTime`=:InTime, `OutTime`=:OutTime, `Status`=:Status WHERE (`ID`=:IDMin);");
                    query->bindValue(":IDMin",IDMin);
                }
                query->bindValue(":ID",IDMax+1);
                query->bindValue(":LineID",LineID);
                query->bindValue(":PINCode",pinCode);
                query->bindValue(":G9",G9);
                query->bindValue(":carType",carType);
                query->bindValue(":InTime",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                query->bindValue(":OutTime","");
                query->bindValue(":Status",Status);
                if(query->exec()){
                    qDebug()<<"insert carInfo query:"<<myQueryFromBoundValues(query->lastQuery(),query->boundValues());
//                    *requestID = RecordID;//一次有效存储便更新requestID以便下次请求数据
                    conf->setValue("baseinfo/requestID",RecordID);//一次有效存储便更新requestID以便下次请求数据
                    if(rfidList.contains(pinCode)){//存储数据的时候rfid表已经有数据，存储成功可以清理掉了
                        lineIDList.removeAt(rfidList.indexOf(pinCode));
                        rfidList.removeAll(pinCode);
                    }
                }else{
                    qDebug()<<"insert carInfo error:"<<query->lastError();
                }
            }else if(Status == "OUT"){//出站小车存储过程
                query->prepare("UPDATE carInfo SET OutTime = :OutTime , `Status` = 'OUT' WHERE PINCode = :PINCode "
                               "AND OutTime = '0000-00-00 00:00:00' AND `Status` = 'IN';");
                query->bindValue(":OutTime",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
                query->bindValue(":PINCode",pinCode);
                if(query->exec()){
                    conf->setValue("baseinfo/requestID",RecordID);//一次有效存储便更新requestID以便下次请求数据
                    qDebug()<<"UPDATE OutTime:"<<myQueryFromBoundValues(query->lastQuery(),query->boundValues());
                }else{
                    qDebug()<<"UPDATE OutTime error:"<<query->lastError();
                }
            }
            //        }
        }
        sqlClose();
    }
    //接口数据处理完，刷新下从数据库中查询当前进站小车所有的车型
    selectAllCarType();
}


//从数据库中查询当前进站小车所有的车型
void sqlQuery::selectAllCarType()
{
    if(sqlOpen()){
        //查询当前数据库内存储的车型信息列表，发到窗口线程做显示
        QList<QVariantMap>list;
        query->exec("SELECT LineID,G9 FROM carInfo WHERE `Status` = 'IN';");
        while (query->next()) {
            QVariantMap map;
            map.insert("LineID",query->value(0));
            map.insert("carType",query->value(1));
            list<<map;
        }
//        qDebug()<<"sendCurrentCarTypeList:"<<list;
        emit sendCurrentCarTypeList(list);//从数据库中查询当前进站小车所有的车型
    }
    sqlClose();
}


void sqlQuery::getRFIDCodeList(QString pin)
{
    qDebug()<<"getRFIDCodeList:"<<pin;
    QString lineID = pin.mid(28,1);
    QString pinCode = pin.mid(2,14);
    QString G9Code = pin.mid(16,5);
    if(sqlOpen()){
        query->prepare("UPDATE carInfo SET LineID = :LineID , G9 = :G9 WHERE LineID = 0 AND PINCode =:PINCode; ");
        query->bindValue(":LineID",lineID);
        query->bindValue(":G9",G9Code);
        query->bindValue(":PINCode",pinCode);
        if(query->exec()){
            qDebug()<<"SET LineID:"<<myQueryFromBoundValues(query->lastQuery(),query->boundValues());
        }
    }
    qDebug()<<"SET LineID numRowsAffected:"<<query->numRowsAffected();//-1：语句错误  0：修改0条结果  1：修改1条结果
    if(query->numRowsAffected() <= 0){
        lineIDList<<lineID;
        rfidList<<pinCode;
        G9List<<G9Code;
    }

    sqlClose();
}

