#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QScrollBar>
#include <QSettings>
#include <QList>
#include <QPair>
#include <QGraphicsOpacityEffect>
#include "confirmationbox.h"
#include <QLabel>
#include <QProcess>
#include "sqlquery.h"

namespace Ui {
class settingsWindow;
}

class settingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit settingsWindow(QWidget *parent = 0);
    ~settingsWindow();

private slots:
    void on_but_close_clicked();

    void on_but_add_clicked();

    void on_but_delete_clicked();

    void on_but_cancel_clicked();

    void on_but_save_clicked();

    void getCarTypeDataList(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >,QVariantMap);
    void slot_getSaveResult(bool);
    void on_but_calibration_clicked();

    void on_but_clearData_clicked();

    void but_clearDataShowAndHide();

signals:
    void sendClose();
    void sendReload();
    void sendCancel();
    void sendQueryStr(QString);
private:
    Ui::settingsWindow *ui;
    sqlQuery * query;
    QProcess *process;
    QSettings *conf;
    QGraphicsOpacityEffect * effect;
    QLabel *label_black;
    confirmationBox *confBox;
    QScrollBar * scrollBar_1;
    QScrollBar * scrollBar_2;
    QList<QLineEdit*>carTypeList_L1;
    QList<QLineEdit*>codeTypeList_L1;
    QList<QLineEdit*>percentList_L1;
    QList<QLineEdit*>carTypeList_L2;
    QList<QLineEdit*>codeTypeList_L2;
    QList<QLineEdit*>percentList_L2;
    void carTypeListIni();
    void createCarTypeForm(int,int,QVariantMap);
    void deleteCarTypeForm(int,int);
    void createMsgBoxIni();
    void setNetworkFunc();
    void getNetworkInfo();
};

#endif // SETTINGSWINDOW_H
