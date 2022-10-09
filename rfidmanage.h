#ifndef RFIDMANAGE_H
#define RFIDMANAGE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTcpSocket>
#include <QSettings>
#include <QDomDocument>

class rfidManage : public QObject
{
    Q_OBJECT
public:
    explicit rfidManage(QObject *parent = 0);
    ~rfidManage();
signals:

public slots:

private slots:
    void rfidInit();
    void connectRfid();
    void disConnectRfid();
    void readRfid();
    void receiversRfid();
    void writeRfid(QString);
    void displayError(QAbstractSocket::SocketError);
private:
    QThread* thread;
    QTimer* timer1;
    QTcpSocket* socket;
    int getRfidConnected();
    void parseReadTagData(QDomElement);
    QString readData;
    QSettings *conf;
};

#endif // RFIDMANAGE_H
