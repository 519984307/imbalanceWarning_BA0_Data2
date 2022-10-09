#ifndef UPDATETIME_H
#define UPDATETIME_H

#include <QObject>
#include <QThread>

class updateTime : public QObject
{
    Q_OBJECT
public:
    explicit updateTime(QObject *parent = 0);

signals:

public slots:
private slots:
    void updateTimeFunc();
    void slot_getDateTime(QString);
private:
    QThread *thread;
    QString lastTime;
    QString currentTime;
};

#endif // UPDATETIME_H
