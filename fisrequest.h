#ifndef FISREQUEST_H
#define FISREQUEST_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QTime>
#include <QSettings>

#include "./json/serializer.h"
#include "./json/parser.h"

#include "sqlquery.h"

#include "updatetime.h"

class fisRequest : public QObject
{
    Q_OBJECT
public:
    explicit fisRequest(QObject *parent = 0);
    ~fisRequest();
    sqlQuery * query;

signals:
    void sendRequestResult(QVariantMap);
//    void sendSelectFisData(QString);
    void sendDateTime(QString);
public slots:
private slots:
    void fisRequestIni();
    void requestRun();
private:
    updateTime *updtTime;
    QThread thread;
    QSettings *conf;
    QNetworkAccessManager *manager;
    QNetworkRequest req;
    QString url;
    QString DataServerIp;
    QString ServerPort;
//    QVariantMap dataMap;
    int pinCodeEnd;
    void myRandCarInfo();
    void httpRequest(QString,QVariantMap);
//    int requestID;
};

#endif // FISREQUEST_H
