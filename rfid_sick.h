#ifndef RFID_SICK_H
#define RFID_SICK_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include <QSettings>

class rfid_SICK : public QObject
{
    Q_OBJECT
public:
    explicit rfid_SICK(QObject *parent = 0);
    ~rfid_SICK();
signals:
    void sendRFIDPINCode(QString);
public slots:

private slots:
    void connectRfid();
    void receiversRfid();
private:
    QThread* thread;
    QTcpSocket* socket;
    QSettings *conf;
    QByteArray readData;
    QString myLongStringToAscii(QString);
};

#endif // RFID_SICK_H
