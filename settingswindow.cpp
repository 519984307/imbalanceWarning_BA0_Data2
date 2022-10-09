#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QDebug>
#include <QTimer>

settingsWindow::settingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    ui->but_add->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->but_cancel->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->but_close->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->but_delete->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->but_save->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->label_flushMsg->hide();
    conf = new QSettings("/config.ini", QSettings::IniFormat);
    process = new QProcess;
    query = new sqlQuery(this);
    connect(this,SIGNAL(sendQueryStr(QString)),query,SLOT(sqlExec(QString)));
    ui->tabWidget->setCurrentIndex(0);
    scrollBar_1 = new QScrollBar;
    scrollBar_1->setStyleSheet("width:30px;");
    ui->listWidget->setVerticalScrollBar(scrollBar_1);//设置垂直滚动条
    scrollBar_2 = new QScrollBar;
    scrollBar_2->setStyleSheet("width:30px;");
    ui->listWidget_2->setVerticalScrollBar(scrollBar_2);//设置垂直滚动条
    getNetworkInfo();
}

settingsWindow::~settingsWindow()
{
    delete ui;
    conf->deleteLater();
    process->deleteLater();
    scrollBar_1->deleteLater();
    scrollBar_2->deleteLater();
    query->deleteLater();
}

void settingsWindow::on_but_close_clicked()
{
    emit sendClose();
}

//创建车型列表初始化
void settingsWindow::carTypeListIni()
{
    QVariantMap mapIni;
    mapIni.insert("carType","车型");
    mapIni.insert("codeType","特征码");
    mapIni.insert("percent","百分比");
    if(1){
        int delCount = carTypeList_L1.size();
        for(int i=delCount;i>0;i--){
            ui->listWidget->takeItem(i-1);
        }
        carTypeList_L1.clear();
        codeTypeList_L1.clear();
        percentList_L1.clear();

        createCarTypeForm(1,1,mapIni);
        carTypeList_L1[0]->setReadOnly(true);
        codeTypeList_L1[0]->setReadOnly(true);
        percentList_L1[0]->setReadOnly(true);
    }
    if(1){
        int delCount = carTypeList_L2.size();
        for(int i=delCount;i>0;i--){
            ui->listWidget_2->takeItem(i-1);
        }
        carTypeList_L2.clear();
        codeTypeList_L2.clear();
        percentList_L2.clear();
        createCarTypeForm(2,1,mapIni);
        carTypeList_L2[0]->setReadOnly(true);
        codeTypeList_L2[0]->setReadOnly(true);
        percentList_L2[0]->setReadOnly(true);
    }
}

//创建车型列表的控件
void settingsWindow::createCarTypeForm(int type,int rows,QVariantMap map)
{
    for(int i=0;i<rows;i++){
        QWidget *itemWidget = new QWidget;
        itemWidget->setGeometry(0,0,540,40);
        QLineEdit *carType = new QLineEdit(itemWidget);
        carType->setAlignment( Qt::AlignHCenter);
        carType->setGeometry(5,0,130,40);
        QLineEdit *codeType = new QLineEdit(itemWidget);
        codeType->setAlignment( Qt::AlignHCenter);
        codeType->setGeometry(5+130+5,0,310,40);
        QLineEdit *percent = new QLineEdit(itemWidget);
        percent->setAlignment( Qt::AlignHCenter);
        percent->setGeometry(5+130+5+310+5,0,65,40);
        carType->setText(map.value("carType").toString());
        codeType->setText(map.value("codeType").toString());
        percent->setText(map.value("percent").toString());
        if(1 == type){
            ui->listWidget->addItem("");
            ui->listWidget->setItemWidget(ui->listWidget->item(carTypeList_L1.size()),itemWidget);
            carTypeList_L1<<carType;
            codeTypeList_L1<<codeType;
            percentList_L1<<percent;
        }else if(2 == type){
            ui->listWidget_2->addItem("");
            ui->listWidget_2->setItemWidget(ui->listWidget_2->item(carTypeList_L2.size()),itemWidget);
            carTypeList_L2<<carType;
            codeTypeList_L2<<codeType;
            percentList_L2<<percent;
        }
    }
}

void settingsWindow::deleteCarTypeForm(int type, int index)
{
    if(1 == type){
        ui->listWidget->takeItem(index);
//        ui->listWidget->removeItemWidget(ui->listWidget->item(index));
        carTypeList_L1.removeAt(index);
        codeTypeList_L1.removeAt(index);
        percentList_L1.removeAt(index);
    }else if(2 == type){
        ui->listWidget_2->takeItem(index);
        carTypeList_L2.removeAt(index);
        codeTypeList_L2.removeAt(index);
        percentList_L2.removeAt(index);
    }
}

//添加车型
void settingsWindow::on_but_add_clicked()
{
    QVariantMap map;
    createCarTypeForm(ui->tabWidget->currentIndex()+1,1,map);
}
//删除车型
void settingsWindow::on_but_delete_clicked()
{
    int type = ui->tabWidget->currentIndex()+1;
    int index = 0;
    if(1 == type){
        index = ui->listWidget->currentRow();
    }else if(2 == type){
        index = ui->listWidget_2->currentRow();
    }
    if(index > 0){
        deleteCarTypeForm(type,index);
    }
}
//取消操作
void settingsWindow::on_but_cancel_clicked()
{
    getNetworkInfo();
    emit sendCancel();
}
//保存设置
void settingsWindow::on_but_save_clicked()
{
    createMsgBoxIni();
    confBox = new confirmationBox(this);
    confBox->setGeometry(150,210,300,180);
    connect(confBox,SIGNAL(sendSelectedResult(bool)),this,SLOT(slot_getSaveResult(bool)));
    confBox->show();
}

//确认保存结果，保存到配置文件
void settingsWindow::slot_getSaveResult(bool result)
{
    effect->deleteLater();
    label_black->deleteLater();
    confBox->deleteLater();
    if(result){
        QStringList allKeys = conf->allKeys();
        for(int i=0;i<allKeys.size();i++){
            QString keyName = allKeys[i];
            if(keyName.contains("carType_L")){
                conf->remove(keyName);
            }
        }
//        if(ui->tabWidget->currentIndex() == 0){
            for(int i=1;i<ui->listWidget->count();i++){
                conf->setValue("carType_L1_"+QString::number(i)+"/carType",carTypeList_L1[i]->text());
                conf->setValue("carType_L1_"+QString::number(i)+"/codeType",codeTypeList_L1[i]->text());
                conf->setValue("carType_L1_"+QString::number(i)+"/percent",percentList_L1[i]->text());
            }
//        }else if(ui->tabWidget->currentIndex() == 1){
            for(int i=1;i<ui->listWidget_2->count();i++){
                conf->setValue("carType_L2_"+QString::number(i)+"/carType",carTypeList_L2[i]->text());
                conf->setValue("carType_L2_"+QString::number(i)+"/codeType",codeTypeList_L2[i]->text());
                conf->setValue("carType_L2_"+QString::number(i)+"/percent",percentList_L2[i]->text());
            }
//        }else if(ui->tabWidget->currentIndex() == 2){
            setNetworkFunc();
//        }
    }
    emit sendReload();
}

//从配置文件中获取的车型信息
void settingsWindow::getCarTypeDataList(QList<QPair<QString,QVariantMap> >list1,QList<QPair<QString,QVariantMap> >list2,QVariantMap netMap)
{
    carTypeListIni();
    for(int i=0;i<list1.size();i++){
        createCarTypeForm(1,1,list1[i].second);
    }
    for(int i=0;i<list2.size();i++){
        createCarTypeForm(2,1,list2[i].second);
    }
//    qDebug()<<"netMap:"<<netMap;
//    ui->ssid->setText(netMap.value("SSID").toString());
//    ui->appswd->setText(netMap.value("psk").toString());
//    ui->ap_ip->setText(netMap.value("WirelessIp").toString());
//    ui->local_ip_1->setText(netMap.value("LocalIp").toString());
//    ui->local_ip_2->setText(netMap.value("LocalIp2").toString());
//    ui->netmask->setText(netMap.value("netmask").toString());
//    ui->gateway->setText(netMap.value("gateway").toString());
//    ui->server_ip->setText(netMap.value("DataServerIp").toString());
//    ui->server_port->setText(netMap.value("ServerPort").toString());
//    ui->rfid_ip->setText(netMap.value("RfidIp").toString());
//    ui->rfid_port->setText(netMap.value("RfidPort").toString());
}

void settingsWindow::createMsgBoxIni()
{
    effect = new QGraphicsOpacityEffect(this);
    label_black = new QLabel(this);
    effect->setOpacity(0.5);
    label_black->setGraphicsEffect(effect);
    label_black->setStyleSheet("background-color: rgb(0,0,0);");
    label_black->setGeometry(0,0,600,600);
    label_black->show();
}

void settingsWindow::on_but_calibration_clicked()
{
    process->execute("rm /etc/pointercal");//阻塞进程，等任务执行结束
    process->start("/usr/local/tslib-instal/bin/ts_calibrate");//不阻塞进程，可以多任务执行
}

void settingsWindow::getNetworkInfo()
{
    ui->ssid->setText(conf->value("baseinfo/SSID").toString());
    ui->appswd->setText(conf->value("baseinfo/psk").toString());
    ui->ap_ip->setText(conf->value("baseinfo/WirelessIp").toString());
    ui->local_ip_1->setText(conf->value("baseinfo/LocalIp").toString());
    ui->local_ip_2->setText(conf->value("baseinfo/LocalIp2").toString());
    ui->netmask->setText(conf->value("baseinfo/netmask").toString());
    ui->gateway->setText(conf->value("baseinfo/gateway").toString());
    ui->server_ip->setText(conf->value("baseinfo/DataServerIp").toString());
    ui->server_port->setText(conf->value("baseinfo/ServerPort").toString());
    ui->rfid_ip->setText(conf->value("baseinfo/RfidIp").toString());
    ui->rfid_port->setText(conf->value("baseinfo/RfidPort").toString());

    ui->warning_setVal->setText(conf->value("baseinfo/warning_setVal").toString());
    ui->alarm_setVal->setText(conf->value("baseinfo/alarm_setVal").toString());
}

void settingsWindow::setNetworkFunc()
{
    conf->setValue("baseinfo/SSID",ui->ssid->text());
    conf->setValue("baseinfo/psk",ui->appswd->text());
    conf->setValue("baseinfo/WirelessIp",ui->ap_ip->text());
    conf->setValue("baseinfo/LocalIp",ui->local_ip_1->text());
    conf->setValue("baseinfo/LocalIp2",ui->local_ip_2->text());
    conf->setValue("baseinfo/netmask",ui->netmask->text());
    conf->setValue("baseinfo/gateway",ui->gateway->text());
    conf->setValue("baseinfo/DataServerIp",ui->server_ip->text());
    conf->setValue("baseinfo/ServerPort",ui->server_port->text());
    conf->setValue("baseinfo/RfidIp",ui->rfid_ip->text());
    conf->setValue("baseinfo/RfidPort",ui->rfid_port->text());

    conf->setValue("baseinfo/warning_setVal",ui->warning_setVal->text());
    conf->setValue("baseinfo/alarm_setVal",ui->alarm_setVal->text());
}

//清除数据库车型，重新获取队列
void settingsWindow::on_but_clearData_clicked()
{
    ui->but_clearData->hide();
    ui->label_flushMsg->show();
    emit sendQueryStr("DELETE FROM carInfo;");
    conf->setValue("baseinfo/requestID",-1);
    QTimer::singleShot(1000,this,SLOT(but_clearDataShowAndHide()));
}

void settingsWindow::but_clearDataShowAndHide()
{
    ui->label_flushMsg->hide();
    ui->but_clearData->show();
}
