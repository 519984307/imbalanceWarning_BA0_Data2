#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);//去掉标题栏|让窗口一直保持置顶
    setWindowFlags(Qt::FramelessWindowHint);
    ui->wifi->hide();
    conf = new QSettings("/config.ini", QSettings::IniFormat);
    process = new QProcess;
}

MainWindow::~MainWindow()
{
    delete ui;
    conf->deleteLater();
    process->deleteLater();
}

//车型比例显示进度条等控件初始化
void MainWindow::barIni()
{
//    networkMap.insert("SSID",conf->value("baseinfo/SSID"));
//    networkMap.insert("psk",conf->value("baseinfo/psk"));
//    networkMap.insert("WirelessIp",conf->value("baseinfo/WirelessIp"));
//    networkMap.insert("LocalIp",conf->value("baseinfo/LocalIp"));
//    networkMap.insert("LocalIp2",conf->value("baseinfo/LocalIp2"));
//    networkMap.insert("netmask",conf->value("baseinfo/netmask"));
//    networkMap.insert("gateway",conf->value("baseinfo/gateway"));
//    networkMap.insert("DataServerIp",conf->value("baseinfo/DataServerIp"));
//    networkMap.insert("ServerPort",conf->value("baseinfo/ServerPort"));
//    networkMap.insert("RfidIp",conf->value("baseinfo/RfidIp"));
//    networkMap.insert("RfidPort",conf->value("baseinfo/RfidPort"));
//    qDebug()<<"networkMap.insert:"<<networkMap;

    int count_L1 = 0;
    int count_L2 = 0;
    carTypeListIni_L1.clear();
    carTypeListIni_L2.clear();
    QStringList keyNameList = conf->childGroups();
    for(int i=0;i<keyNameList.size();i++){
        QString keyName = keyNameList.at(i);
        //提取配置文件中L1线的车型信息
        if(keyName.contains("carType_L1_")){
            QVariantMap dataMap;
            QPair<QString,QVariantMap>carTypePair;
            dataMap.insert("carType",conf->value(keyName+"/carType"));
            dataMap.insert("codeType",conf->value(keyName+"/codeType"));
            dataMap.insert("percent",conf->value(keyName+"/percent"));
            carTypePair.first = keyName.remove("carType_L1_");
            carTypePair.second = dataMap;
            carTypeListIni_L1<<carTypePair;
            count_L1++;
        }
        //提取配置文件中L2线的车型信息
        if(keyName.contains("carType_L2_")){
            QVariantMap dataMap;
            QPair<QString,QVariantMap>carTypePair;
            dataMap.insert("carType",conf->value(keyName+"/carType"));
            dataMap.insert("codeType",conf->value(keyName+"/codeType"));
            dataMap.insert("percent",conf->value(keyName+"/percent"));
            carTypePair.first = keyName.remove("carType_L2_");
            carTypePair.second = dataMap;
            carTypeListIni_L2<<carTypePair;
            count_L2++;
        }
    }
//    qDebug()<<"currentSetCar:"<<carTypeListIni_L1<<carTypeListIni_L2;

    for(int i=0;i<barList_l1.size();i++){
        barList_l1[i]->deleteLater();
        labelList_l1[i]->deleteLater();
        labelList_2_l1[i]->deleteLater();
        label_count_l1[i]->deleteLater();
    }
    for(int i=0;i<barList_l2.size();i++){
        barList_l2[i]->deleteLater();
        labelList_l2[i]->deleteLater();
        labelList_2_l2[i]->deleteLater();
        label_count_l2[i]->deleteLater();
    }
    barList_l1.clear();
    labelList_l1.clear();
    labelList_2_l1.clear();
    label_count_l1.clear();
    barList_l2.clear();
    labelList_l2.clear();
    labelList_2_l2.clear();
    label_count_l2.clear();
    displayModuleIni(1,count_L1);
    displayModuleIni(2,count_L2);

    warningAnalysis();
}

//显示模块初始化 LNUM:哪条线 count:几种车型
void MainWindow::displayModuleIni(int LNUM, int count)
{
    int allCount_l1 = 0;
    int allCount_l2 = 0;
    if(1 == LNUM){ //计算车型总数
        for(int i=0;i<carTypeList_l1.second.size();i++){
            int count = carTypeList_l1.first[i];
            allCount_l1 += count;
        }
    }else if(2 == LNUM){
        for(int i=0;i<carTypeList_l2.second.size();i++){
            int count = carTypeList_l2.first[i];
            allCount_l2 += count;
        }
    }


    QList<int>percent_l1; //记录1线各车型的百分比
    QList<int>percent_l2; //记录2线各车型的百分比
    for(int i=0;i<count;i++){
        if(1 == LNUM && allCount_l1 >0){
            QProgressBar *bar = new QProgressBar(ui->groupBox);
            barList_l1<<bar;
            QLabel *label = new QLabel(ui->groupBox);
            labelList_l1<<label;
            QLabel *label_2 = new QLabel(ui->groupBox);
            labelList_2_l1<<label_2;
            QLabel *label_count = new QLabel(ui->groupBox);
            label_count_l1<<label_count;
            int currentVal = 0;
            QString currentCarType = carTypeListIni_L1[i].second.value("carType").toString();
            QString currentCar = carTypeListIni_L1[i].second.value("codeType").toString();
            QStringList currentCarList = currentCar.split(QRegExp("[ ,]"));
            for(int k=0;k<currentCarList.size();k++){
                QString code = currentCarList.at(k);
                if(code.contains("?")){
                    QStringList containsList = myContains(carTypeList_l1.second,code);
                    for(int j=0;j<containsList.size();j++){
                        QString code2 = containsList.at(j);
                        currentVal += carTypeList_l1.first[carTypeList_l1.second.indexOf(code2)]*100; //一个车型可能设置了多个匹配项，要加起来
                    }
                }else{
                    if(carTypeList_l1.second.contains(code)){
                        currentVal += carTypeList_l1.first[carTypeList_l1.second.indexOf(code)]*100; //一个车型可能设置了多个匹配项，要加起来
                    }
                }
            }
            percent_l1.append(qRound(currentVal/allCount_l1));
            label_count->setText(QString::number(currentVal/100));
            label_2->setText(currentCarType);
//            mysetStyleSheet(label_count,bar,label,label_2,count,i,setVal,currentVal/allCount_l1);
        }
        if(2 == LNUM && allCount_l2 > 0){
            QProgressBar *bar = new QProgressBar(ui->groupBox_2);
            bar->setValue(carTypeListIni_L2[i].second.value("percent").toInt());
            barList_l2<<bar;
            QLabel *label = new QLabel(ui->groupBox_2);
            labelList_l2<<label;
            QLabel *label_2 = new QLabel(ui->groupBox_2);
            labelList_2_l2<<label_2;
            QLabel *label_count = new QLabel(ui->groupBox_2);
            label_count_l2<<label_count;
            int currentVal = 0;
            QString currentCarType = carTypeListIni_L2[i].second.value("carType").toString();
            QString currentCar = carTypeListIni_L2[i].second.value("codeType").toString();
            QStringList currentCarList = currentCar.split(QRegExp("[ ,]"));

            for(int k=0;k<currentCarList.size();k++){
                QString code = currentCarList.at(k);
                if(code.contains("?")){
                    QStringList containsList = myContains(carTypeList_l2.second,code);
                    for(int j=0;j<containsList.size();j++){
                        QString code2 = containsList.at(j);
                        currentVal += carTypeList_l2.first[carTypeList_l2.second.indexOf(code2)]*100; //一个车型可能设置了多个匹配项，要加起来
                    }
                }else{
                    if(carTypeList_l2.second.contains(code)){
                        currentVal += carTypeList_l2.first[carTypeList_l2.second.indexOf(code)]*100; //一个车型可能设置了多个匹配项，要加起来
                    }
                }
            }
            percent_l2.append(qRound(currentVal/allCount_l2));
            label_count->setText(QString::number(currentVal/100));
            label_2->setText(currentCarType);
//            mysetStyleSheet(label_count,bar,label,label_2,count,i,setVal,currentVal/allCount_l2);
        }
    }

    if(1 == LNUM){
        // 计算百分比，百分比和一定是100
        int index_max = -1;
        int max = -1;
        int sum = 0;
        if(percent_l1.size() > 0){
            max = percent_l1[0];
            sum = percent_l1[0];
            for(int i = 1;i<percent_l1.size();i++){
                if(max < percent_l1[i]){
                    max = percent_l1[i];
                }
                sum += percent_l1[i];
            }
            index_max = percent_l1.indexOf(max);
            percent_l1[index_max] += 100 - sum;
        }

        // 处理界面的图形及数据显示
        for(int i = 0;i<barList_l1.size();i++){
            //获取设置的百分比
            int setVal = carTypeListIni_L1[i].second.value("percent").toInt();
            mysetStyleSheet(label_count_l1[i],barList_l1[i],labelList_l1[i],labelList_2_l1[i],count,i,setVal,percent_l1[i]);
        }
    }else if(2 == LNUM){
        // 计算百分比，百分比和一定是100
        int index_max = -1;
        int max = -1;
        int sum = 0;
        if(percent_l2.size() > 0){
            max = percent_l2[0];
            sum = percent_l2[0];
            for(int i = 1;i<percent_l2.size();i++){
                if(max < percent_l2[i]){
                    max = percent_l2[i];
                }
                sum += percent_l2[i];
            }
            index_max = percent_l2.indexOf(max);
            percent_l2[index_max] += 100 - sum;
        }

        // 处理界面的图形及数据显示
        for(int i = 0;i<barList_l2.size();i++){
            //获取设置的百分比
            int setVal = carTypeListIni_L2[i].second.value("percent").toInt();
            mysetStyleSheet(label_count_l2[i],barList_l2[i],labelList_l2[i],labelList_2_l2[i],count,i,setVal,percent_l2[i]);
        }
    }

}


//设置显示模块控件的样式表
void MainWindow::mysetStyleSheet(QLabel *label_count,QProgressBar *bar,QLabel *label,QLabel *label_2,int count/*共几个进度条*/,int i/*第几个进度条*/,int setVal/*设置的占比值*/,int currentVal/*当前的显示占比*/)
{
//    qDebug()<<"mysetStyleSheet";

    label_count->setGeometry(130+((ui->groupBox->width()-130)/count)*i,40,61,30);
    label_count->setStyleSheet("font: 14pt \"黑体\";");

    bar->setOrientation(Qt::Vertical);
    bar->setValue(currentVal);
    bar->setGeometry(130+((ui->groupBox->width()-130)/count)*i,70,61,170);
    bar->setStyleSheet("QProgressBar{border: 1px solid  rgb(51, 153, 255); font: 14pt \"黑体\";}QProgressBar::chunk{background:rgb(38, 115, 191);}");

    label->setText(QString::number(setVal)+"%");
    label->setGeometry(130+((ui->groupBox->width()-130)/count)*i,240,61,30);
    label->setStyleSheet("font: 14pt \"黑体\";");

    label_2->setGeometry(130+((ui->groupBox->width()-130)/count)*i,280,61,30);
    label_2->setStyleSheet("font: 14pt \"黑体\";");
    QFontMetrics fontWidth(label_2->font());
    label_2->setFixedWidth(fontWidth.boundingRect(label_2->text()).width());

    label_count->show();
    bar->show();
    label->show();
    label_2->show();
}



//报警分析
void MainWindow::warningAnalysis()
{
    whichLED = "g_led";
    for(int i=0;i<barList_l1.size();i++){
        double setVal = labelList_l1[i]->text().remove("%").toDouble();
        double ratio = 0;
        if(setVal > 0){
            ratio = (barList_l1[i]->value()-setVal)/setVal;
            displayWarning(ratio,barList_l1[i]);
        }else{
            displayWarning(1,barList_l1[i]);
        }
    }
    for(int i=0;i<barList_l2.size();i++){
        double setVal = labelList_l2[i]->text().remove("%").toDouble();
        double ratio = 0;
        if(setVal > 0){
            ratio = (barList_l2[i]->value()-setVal)/setVal;
            displayWarning(ratio,barList_l2[i]);
        }else{
            displayWarning(1,barList_l2[i]);
        }
    }

    if(whichLED == "r_led"){
        system("echo 1 > /sys/class/leds/control_led2/brightness &");
        system("echo 0 > /sys/class/leds/control_led4/brightness &");
    }else if(whichLED == "y_led"){
        system("echo 1 > /sys/class/leds/control_led4/brightness &");
        system("echo 0 > /sys/class/leds/control_led2/brightness &");
    }else{
        system("echo 0 > /sys/class/leds/control_led2/brightness &");
        system("echo 0 > /sys/class/leds/control_led4/brightness &");
    }
    qDebug()<<"whichLED:"<<whichLED;
}

//按当前车辆比例情况来显示报警信息（颜色）
void MainWindow::displayWarning(double ratio,QProgressBar *bar)
{
    float warning_setVal = conf->value("baseinfo/warning_setVal").toFloat();
    float alarm_setVal = conf->value("baseinfo/alarm_setVal").toFloat();
    if(ratio > alarm_setVal/100){
        whichLED = "r_led";
        bar->setStyleSheet("QProgressBar{border: 1px solid  rgb(51, 153, 255); font: 14pt \"黑体\";}QProgressBar::chunk{background:rgb(250, 0, 0);}");
    }else if(ratio > warning_setVal/100){
        if(whichLED != "r_led"){
            whichLED = "y_led";
        }
        bar->setStyleSheet("QProgressBar{border: 1px solid  rgb(51, 153, 255); font: 14pt \"黑体\";}QProgressBar::chunk{background:rgb(250, 206, 16);}");
    }else{
       bar->setStyleSheet("QProgressBar{border: 1px solid  rgb(51, 153, 255); font: 14pt \"黑体\";}QProgressBar::chunk{background:rgb(38, 115, 191);}");
    }
}

//设置
void MainWindow::on_but_set_clicked()
{
   createPWD("settingsWindow");
}

//查询
void MainWindow::on_but_query_clicked()
{
    queryDia = new queryDialog;
    connect(queryDia,SIGNAL(sendClose()),this,SLOT(slot_closeQueryDialot()));
    connect(this,SIGNAL(sendCarTypeList(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >)),queryDia,SLOT(searchIni(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >)));

    QStringList carType_l1;//1线的所有车型
    QStringList carType_l2;//2线的所有车型
    for(int i=0;i<carTypeListIni_L1.size();i++){
        carType_l1<<carTypeListIni_L1[i].second.value("carType").toString();
    }
    for(int i=0;i<carTypeListIni_L2.size();i++){
        carType_l2<<carTypeListIni_L2[i].second.value("carType").toString();
    }
    emit sendCarTypeList(carTypeListIni_L1,carTypeListIni_L2);//将配置文件中的车型数据发到查询窗口
    queryDia->show();
}

void MainWindow::slot_closeQueryDialot()
{
    queryDia->deleteLater();
}


void MainWindow::slot_pswResult(bool result,QString nextWin)
{
    closePWD(result,nextWin);
}

void MainWindow::createPWD(QString nextWin)
{
    createMsgBoxIni();
    pswdBox = new PasswordPanel(this);
    pswdBox->nextWin = nextWin;
    pswdBox->show();
    connect(pswdBox,SIGNAL(sendResult(bool,QString)),this,SLOT(slot_pswResult(bool,QString)));
}

void MainWindow::createMsgBoxIni()
{
    effect = new QGraphicsOpacityEffect(this);
    label_black = new QLabel(this);
    effect->setOpacity(0.5);
    label_black->setGraphicsEffect(effect);
    label_black->setStyleSheet("background-color: rgb(0,0,0);");
    label_black->setGeometry(0,0,1366,768);
    label_black->show();
}

//关闭密码输入界面的操作
void MainWindow::closePWD(bool result,QString nextWin)
{
    closeMsgBox();
    pswdBox->deleteLater();
    if(result){
        if("settingsWindow" == nextWin){
            createMsgBoxIni();
            setWin = new settingsWindow(this);
            connect(setWin,SIGNAL(sendReload()),this,SLOT(barIni()));
            connect(setWin,SIGNAL(sendClose()),this,SLOT(slot_closeSetWindow()));
            connect(setWin,SIGNAL(sendCancel()),this,SLOT(slot_cancelFunc()));
            connect(this,SIGNAL(sendCarDatas(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >,QVariantMap)),
                    setWin,SLOT(getCarTypeDataList(QList<QPair<QString,QVariantMap> >,QList<QPair<QString,QVariantMap> >,QVariantMap)));
//            qDebug()<<"networkMap:"<<networkMap;
            emit sendCarDatas(carTypeListIni_L1,carTypeListIni_L2,networkMap);//密码输入正确把配置文件数据发到设置窗口
            setWin->show();
        }
    }
}

//取消的时候把配置文件原始数据发到设置窗口
void MainWindow::slot_cancelFunc()
{
    emit sendCarDatas(carTypeListIni_L1,carTypeListIni_L2,networkMap);
}

void MainWindow::closeMsgBox()
{
    effect->deleteLater();
    label_black->deleteLater();
}

//
void MainWindow::slot_closeSetWindow()
{
    closeMsgBox();
    setWin->deleteLater();
}

//void MainWindow::slot_getRequestResult(QVariantMap result)
//{
//    result[""];
//    result[""];
//    result[""];
//    result[""];
//    result[""];
//    result[""];
//}

//列表去重
void MainWindow::removeListSame(QStringList *list)
{
    for (int i = 0; i < list->count(); i++){
        for (int k = i + 1; k <  list->count(); k++){
            if ( list->at(i) ==  list->at(k)){
                list->removeAt(k);
                k--;
            }
        }
    }
}

//接收从数据库中查询当前进站小车所有的车型
void MainWindow::slot_currentCarTypeList(QList<QVariantMap>list)
{
    carTypeList_l1.first.clear();
    carTypeList_l1.second.clear();
    carTypeList_l2.first.clear();
    carTypeList_l2.second.clear();
//    carTypeListIni_L1;
//    carTypeListIni_L2;
    for(int i=0;i<list.size();i++){
        QVariantMap carInfo = list.value(i);
        int LineID = carInfo["LineID"].toInt();
        QString carType = carInfo["carType"].toString();
        if(i>0){
            if(1 == LineID){
                if(carTypeList_l1.second.contains(carType)){
                    carTypeList_l1.first[carTypeList_l1.second.indexOf(carType)]++;
                }else{
                    carTypeList_l1.second<<carType;
                    carTypeList_l1.first<<1;
                }
            }else if(2 == LineID){
                if(carTypeList_l2.second.contains(carType)){
                    carTypeList_l2.first[carTypeList_l2.second.indexOf(carType)]++;
                }else{
                    carTypeList_l2.second<<carType;
                    carTypeList_l2.first<<1;
                }
            }
        }else{
            if(1 == LineID){
                carTypeList_l1.first<<1;
                carTypeList_l1.second<<carType;
            }else if(2 == LineID){
                carTypeList_l2.first<<1;
                carTypeList_l2.second<<carType;
            }
        }
    }

//    qDebug()<<"currentCarTypeList:"<<carTypeList_l1<<carTypeList_l2;
    barIni();
}


void MainWindow::on_but_close_clicked()
{
    effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.5);
    but_blur = new QPushButton(this);
    but_blur->setFocusPolicy(Qt::NoFocus);
    but_blur->setGraphicsEffect(effect);
    but_blur->setStyleSheet("background-color: rgb(0,0,0);");
    but_blur->setGeometry(0,0,1366,768);
    but_blur->show();
    but_reboot= new QPushButton(this);
    but_reboot->setFocusPolicy(Qt::NoFocus);
    but_reboot->setGeometry((1366/2-200)/2+1366/2,(768-200)/2,200,200);
    but_reboot->setText(tr("重新启动"));
    but_reboot->setStyleSheet("border-style: solid;border-radius:20px;background-color: rgb(241, 156, 31); font: 20pt \"黑体\";color: rgb(248, 248, 255);");
    but_reboot->show();
    connect(but_reboot,SIGNAL(clicked()),this,SLOT(reboot()));
    but_poweroff= new QPushButton(this);
    but_poweroff->setFocusPolicy(Qt::NoFocus);
    but_poweroff->setGeometry((1366/2-200)/2,(768-200)/2,200,200);
    but_poweroff->setText(tr("关闭电源"));
    but_poweroff->setStyleSheet("border-style: solid;border-radius:20px;background-color: rgb(255, 102, 102); font: 20pt \"黑体\";color: rgb(248, 248, 255);");
    but_poweroff->show();
    connect(but_poweroff,SIGNAL(clicked()),this,SLOT(poweroff()));
    connect(but_blur,SIGNAL(clicked()),this,SLOT(closeDialog()));
}

void MainWindow::reboot()
{
    qDebug() << "reboot process";
    process->execute("reboot");
}

void MainWindow::poweroff()
{
    qDebug() << "poweroff process";
    process->execute("echo 0 > /sys/class/leds/control_lvds/brightness");
    process->execute("poweroff");
}

void MainWindow::closeDialog()
{
    effect->deleteLater();
    but_blur->deleteLater();
    but_reboot->deleteLater();
    but_poweroff->deleteLater();
}

void MainWindow::slot_getDateTime(QString dateTime)
{
    ui->show_time->setText(dateTime);

    QFile file("/var/test_wifi");
    QString msg;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        msg = file.readAll();
        file.close();
    }else{
        qDebug()<<"open test_wifi:"<<file.errorString();
    }
//    qDebug()<<"test_wifi:"<<msg;
    QStringList list = msg.split(" ");
    int state = 0;
    if(list.size() > 1){
        state = list.at(1).toInt();
    }
    if(state == 0){
        ui->wifi->hide();
    }else if(state == 1){
        ui->wifi->show();
    }
}

bool MainWindow::myContains(QString dataA, QString dataB)
{
    int lenA = dataA.size();
    int lenB = dataB.size();
    if(lenA == lenB){
        for(int i = 0;i<lenA;i++){
            if(dataA[i] != dataB[i] && dataB[i] != '?'){
                return false;
            }
        }
        return true;
    }else{
        return false;
    }
}

QStringList MainWindow::myContains(QStringList dataA_list, QString dataB)
{
    QStringList list;
    for(int i = 0;i<dataA_list.size();i++){
        QString dataA = dataA_list.at(i);
        if(myContains(dataA,dataB)){
            list<<dataA;
        }
    }
    return list;
}
