#ifndef MYDATETIME_H
#define MYDATETIME_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDateTime>

class myDateTime : public QObject
{
    Q_OBJECT
public:
    explicit myDateTime(QObject *parent = 0);

signals:
    void sendCurrentDateTime(QString);
public slots:

private slots:
    void getCurrentDateTime();
private:
    QThread *thread;

};

#endif // MYDATETIME_H
