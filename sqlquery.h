#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QObject>
#include <QThread>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStringList>
#include <QSettings>

class sqlQuery : public QObject
{
    Q_OBJECT
public:
    explicit sqlQuery(QObject *parent = 0);
    ~sqlQuery();
    void selectAllCarType();
signals:
    void sendSqlModelData(QSqlQuery*);
    void sendCurrentCarTypeList(QList<QVariantMap>);
public slots:

private slots:
//    void sqlModesIni();
    void sqlExec(QString);
    void insertProcess(QVariantMap);
    void getRFIDCodeList(QString);
private:
    QThread *thread;
    QSqlDatabase sqlDB;
    QSqlQuery *query;
    QSettings *conf;
    void sqlConnect();
    bool sqlOpen();
    void sqlClose();
    QString myQueryFromBoundValues(QString,QVariantMap);
    QStringList lineIDList; //列表存储所有读到的RFID中的线号字段
    QStringList rfidList; //列表存储所有读到的RFID中的PIN码字段
    QStringList G9List; //列表存储所有读到的RFID中的G9码字段
};

#endif // SQLQUERY_H
