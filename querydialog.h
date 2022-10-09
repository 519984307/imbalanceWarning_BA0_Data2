#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include "sqlquery.h"

namespace Ui {
class queryDialog;
}

class queryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit queryDialog(QWidget *parent = 0);
    ~queryDialog();

private slots:
    void on_but_close_clicked();
    void slot_getQueryModelIniData(QSqlQuery*);
    void on_but_search_clicked();
    void searchIni(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >);
    void on_checkBox_L1_clicked(bool checked);

    void on_checkBox_L2_clicked(bool checked);

    void on_but_previous_clicked();

    void on_but_next_clicked();

    void on_but_first_clicked();

    void on_but_last_clicked();

signals:
    void sendClose();
//    void sendQueryIni();
    void sendQueryStr(QString);
private:
    Ui::queryDialog *ui;
    QSqlQueryModel *sqlModel;
    sqlQuery * query;
    QStringList carType_L1;
    QStringList carType_L2;
    int currentPage;
    int resultPage;
    int queryPage;
    void modelLayout(int);
    void removeListSame(QStringList*);
    void search_carTypeManage(QStringList,QStringList);
    void queryFilterManage(int);
    QList<QPair<QString,QVariantMap> > carTypeListIni_L1;//存储配置文件的1线车型数据 string:在配置文件中的编号 varianMap(carType,codeType,percent)
    QList<QPair<QString,QVariantMap> > carTypeListIni_L2;//存储配置文件的2线车型数据 string:在配置文件中的编号 varianMap(carType,codeType,percent)
};

#endif // QUERYDIALOG_H
