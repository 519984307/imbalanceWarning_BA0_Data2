#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include "passwordpanel.h"
#include "settingswindow.h"
#include "querydialog.h"
#include <QGraphicsOpacityEffect>
#include <QSettings>
#include <QPair>
#include <QList>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    queryDialog *queryDia;
public slots:
//    void slot_getRequestResult(QVariantMap);
private slots:
    void slot_currentCarTypeList(QList<QVariantMap>);
    void slot_getDateTime(QString);

    void on_but_set_clicked();

    void on_but_query_clicked();

    void slot_pswResult(bool,QString);
    void slot_closeSetWindow();
    void barIni();
    void slot_cancelFunc();
    void slot_closeQueryDialot();
    void on_but_close_clicked();
    void reboot();
    void poweroff();
    void closeDialog();

signals:
    void sendCarDatas(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >,QVariantMap);
    void sendCarTypeList(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >);
    void sendSelectAllCarType();
private:
    Ui::MainWindow *ui;
    QSettings *conf;
    void displayModuleIni(int,int);
    void mysetStyleSheet(QLabel*,QProgressBar*,QLabel*,QLabel*,int,int,int,int);
    void warningAnalysis();
    void displayWarning(double,QProgressBar*);
    void createMsgBoxIni();
    void createPWD(QString);
    void closePWD(bool,QString);
    void closeMsgBox();
    void removeListSame(QStringList*);
    QString whichLED;
    QList<QProgressBar*>barList_l1;
    QList<QLabel*>labelList_l1;
    QList<QLabel*>labelList_2_l1;
    QList<QLabel*>label_count_l1; //用来显示1线当前车型的数量

    QList<QProgressBar*>barList_l2;
    QList<QLabel*>labelList_l2;
    QList<QLabel*>labelList_2_l2;
    QList<QLabel*>label_count_l2;//用来显示2线当前车型的数量


    QGraphicsOpacityEffect * effect;
    QLabel * label_black;
    PasswordPanel *pswdBox;
    settingsWindow *setWin;
    QList<QPair<QString,QVariantMap> > carTypeListIni_L1;//存储配置文件的1线车型数据 string:在配置文件中的编号 varianMap(carType,codeType,percent)
    QList<QPair<QString,QVariantMap> > carTypeListIni_L2;//存储配置文件的2线车型数据 string:在配置文件中的编号 varianMap(carType,codeType,percent)

    QPair<QList<int>,QStringList>carTypeList_l1;//存储从数据库中查询出的1线车型比例 intList:车型的数据条数 stringList:车型名称
    QPair<QList<int>,QStringList>carTypeList_l2;//存储从数据库中查询出的2线车型比例 intList:车型的数据条数 stringList:车型名称

    QVariantMap networkMap;
    QProcess *process;
    QPushButton *but_blur;
    QPushButton *but_reboot;
    QPushButton *but_poweroff;

    bool myContains(QString,QString);//可以处理含通配符(定义为?)的比较
    QStringList myContains(QStringList,QString);//可以处理含通配符(定义为?)的比较 并将匹配到列表中所有的值生成新的list
};

#endif // MAINWINDOW_H
