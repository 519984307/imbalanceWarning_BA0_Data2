#ifndef LOGDELETE_H
#define LOGDELETE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QDateTime>

class logDelete : public QObject
{
    Q_OBJECT
public:
    explicit logDelete(QObject *parent = 0);
    ~logDelete();
signals:

public slots:
private slots:
    void logDelIni();
    void logDelFunc();
private:
    QThread *thread;
    QTimer *timer;
};

#endif // LOGDELETE_H
