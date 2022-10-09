#include "querydialog.h"
#include "ui_querydialog.h"
#include <QDebug>

queryDialog::queryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::queryDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    ui->but_search->setFocusPolicy(Qt::NoFocus);//

    sqlModel = new QSqlQueryModel(this);
    query = new sqlQuery;
//    connect(this,SIGNAL(sendQueryIni()),query,SLOT(sqlModesIni()));
    connect(this,SIGNAL(sendQueryStr(QString)),query,SLOT(sqlExec(QString)));
    connect(query,SIGNAL(sendSqlModelData(QSqlQuery*)),this,SLOT(slot_getQueryModelIniData(QSqlQuery*)));
//    emit sendQueryIni();
    queryPage = 0;
    sendQueryStr("select ID,LineID,PINCode,G9,DATE_FORMAT(InTime,'%Y-%m-%d %H:%i:%s'),DATE_FORMAT(OutTime,'%Y-%m-%d %H:%i:%s'),Status from carInfo ORDER BY ID DESC LIMIT 0,10;");
}

queryDialog::~queryDialog()
{
    delete ui;
    query->deleteLater();
}

void queryDialog::on_but_close_clicked()
{
    emit sendClose();
}


void queryDialog::modelLayout(int rows)
{
    sqlModel->setHeaderData(0,Qt::Horizontal,"ID");
    sqlModel->setHeaderData(1,Qt::Horizontal,"线号");
    sqlModel->setHeaderData(2,Qt::Horizontal,"PIN码");
    sqlModel->setHeaderData(3,Qt::Horizontal,"车型");
    sqlModel->setHeaderData(4,Qt::Horizontal,"进站时间");
    sqlModel->setHeaderData(5,Qt::Horizontal,"出站时间");
    sqlModel->setHeaderData(6,Qt::Horizontal,"状态");
    ui->tableView->setModel(sqlModel);
    ui->tableView->setColumnWidth(0,100);
    ui->tableView->setColumnWidth(1,60);
    ui->tableView->setColumnWidth(2,219);
    ui->tableView->setColumnWidth(3,200);
    ui->tableView->setColumnWidth(4,300);
    ui->tableView->setColumnWidth(5,300);
    ui->tableView->setColumnWidth(6,100);
    for(int i=0;i<rows;i++){
        ui->tableView->setRowHeight(i,45);
    }
    //    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableView->setShowGrid(false);//不显示表格线
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color: rgb(51, 153, 255);"
                                                         "color: rgb(248, 248, 255);border: 0px; font:14pt}");//表头样式表
    ui->tableView->horizontalHeader()->setFixedHeight(50);//设置表头高度
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);//表头文本 水平左对齐 | 垂直居中
    ui->tableView->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tableView->horizontalHeader()->setVisible(true); //隐藏行表头
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//水平滚动条隐藏
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//垂直滚动条隐藏
}

//接收查询的结果
void queryDialog::slot_getQueryModelIniData(QSqlQuery *query)
{
    if(0 == queryPage){
        int resultCount = query->size();
        ui->label_num->setText(QString::number(resultCount));
        if(resultCount%10){
            resultPage = resultCount/10 + 1;
        }else{
            resultPage = resultCount/10;
        }

        if(0 == resultCount){
            currentPage = 0;
        }else{
            currentPage = 1;
        }
        ui->label_M->setText(QString::number(resultPage));
    }else{
        currentPage = queryPage;
    }
    ui->label_N->setText(QString::number(currentPage));

    sqlModel->setQuery(*query);
    modelLayout(query->size());
}

//列表去重
void queryDialog::removeListSame(QStringList *list)
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

//查询条件中车型选项列表整理
void queryDialog::search_carTypeManage(QStringList carType_l1, QStringList carType_l2)
{
    ui->comboBox_carType->clear();
    ui->comboBox_carType->addItem("");
    QStringList carTypeList;
    if(ui->checkBox_L1->isChecked()){
        carTypeList<<carType_l1;
    }
    if(ui->checkBox_L2->isChecked()){
        carTypeList<<carType_l2;
    }
    removeListSame(&carTypeList);
    ui->comboBox_carType->addItems(carTypeList);
}


//获取到配置文件中所有车型配置信息 string:在配置文件中的编号 varianMap(carType,codeType,percent)
void queryDialog::searchIni(QList<QPair<QString,QVariantMap> > carList_l1,QList<QPair<QString,QVariantMap> > carList_l2)
{
    carTypeListIni_L1 = carList_l1;
    carTypeListIni_L2 = carList_l2;
    for(int i = 0;i<carList_l1.size();i++){
        carType_L1<<carList_l1.at(i).second["carType"].toString();
    }
    for(int i = 0;i<carList_l2.size();i++){
        carType_L2<<carList_l2.at(i).second["carType"].toString();
    }
    search_carTypeManage(carType_L1, carType_L2);
}

//查询条件整理
void queryDialog::queryFilterManage(int page)
{
    queryPage = page;
    QString queryBase = "select ID,LineID,PINCode,G9,DATE_FORMAT(InTime,'%Y-%m-%d %H:%i:%s'),DATE_FORMAT(OutTime,'%Y-%m-%d %H:%i:%s'),Status from carInfo ";
    QString queryFilter1;
    QString queryFilter2;
    QString queryFilter3;
    QString querySort = "ORDER BY ID DESC ";
    QString queryLimit;

    if(ui->checkBox_IN->isChecked() != ui->checkBox_OUT->isChecked()){//进/出站选择
        if(ui->checkBox_IN->isChecked()){
            queryFilter1 = "where Status = \"IN\" ";
        }else{
            queryFilter1 = "where Status = \"OUT\" ";
        }
    }

    QStringList selectCodeTypeList;
    QString choicesCarType = ui->comboBox_carType->currentText();//车型选择

    if(ui->checkBox_L1->isChecked() != ui->checkBox_L2->isChecked()){//一/二线选择
        if(ui->checkBox_L1->isChecked()){
            if(queryFilter1.size() > 0){
                queryFilter2 = "and LineID = \"1\" ";
            }else{
                queryFilter2 = "where LineID = \"1\" ";
            }
            for(int i = 0;i<carTypeListIni_L1.size();i++){
                if(carTypeListIni_L1.at(i).second["carType"].toString() == choicesCarType){
                    selectCodeTypeList<<carTypeListIni_L1.at(i).second["codeType"].toString();
                }
            }
        }else{
            if(queryFilter1.size() > 0){
                queryFilter2 = "and LineID = \"2\" ";
            }else{
                queryFilter2 = "where LineID = \"2\" ";
            }
            for(int i = 0;i<carTypeListIni_L2.size();i++){
                if(carTypeListIni_L2.at(i).second["carType"].toString() == choicesCarType){
                    selectCodeTypeList<<carTypeListIni_L2.at(i).second["codeType"].toString();
                }
            }
        }
    }else{
        for(int i = 0;i<carTypeListIni_L1.size();i++){
            if(carTypeListIni_L1.at(i).second["carType"].toString() == choicesCarType){
                selectCodeTypeList<<carTypeListIni_L1.at(i).second["codeType"].toString();
            }
        }
        for(int i = 0;i<carTypeListIni_L2.size();i++){
            if(carTypeListIni_L2.at(i).second["carType"].toString() == choicesCarType){
                selectCodeTypeList<<carTypeListIni_L2.at(i).second["codeType"].toString();
            }
        }
    }

    QStringList listTemp;
    for(int i = 0;i<selectCodeTypeList.size();i++){//整理查询条件，拆分多个，去掉重复项，替换?为_
        QString code = selectCodeTypeList.at(i);
        code = code.replace("?","_");
        QStringList listSplit = code.split(QRegExp("[ ,]"));
        for(int k=0;k<listSplit.size();k++){
            QString codeNext = listSplit.at(k);
            if(!listTemp.contains(codeNext)){
                listTemp<<codeNext;
            }
        }
    }
    selectCodeTypeList=listTemp;//得到所有codeType的查询值
    qDebug()<<"selectCodeTypeList:"<<selectCodeTypeList;

    if(choicesCarType.size() > 0){
        if(queryFilter1.size() + queryFilter2.size() > 0){//前面进/出站和一/二线只要有做过选择
            //else:如果没填特征码，那就不管这条件
            for(int i=0;i<selectCodeTypeList.size();i++){
                QString carType = selectCodeTypeList.at(i);
                if(carType.contains("_")){
                    queryFilter3 += "and G9 like \"" + carType + "\" ";
                }else{
                    queryFilter3 += "and G9 = \"" + carType + "\" ";
                }
            }
        }else{
            for(int i=0;i<selectCodeTypeList.size();i++){
                QString carType = selectCodeTypeList.at(i);
                if(i == 0){
                    if(carType.contains("_")){
                        queryFilter3 = "where G9 like \"" + carType + "\" ";
                    }else{
                        queryFilter3 = "where G9 = \"" + carType + "\" ";
                    }
                }else{
                    if(carType.contains("_")){
                        queryFilter3 += "and G9 like \"" + carType + "\" ";
                    }else{
                        queryFilter3 += "and G9 = \"" + carType + "\" ";
                    }
                }
            }
        }
    }

    QString displayCount = ui->limited->text();
    if(page > 0){
        queryLimit = "LIMIT " + QString::number((page-1)*10) + ",10 ";
    }else{
        if(displayCount.size() > 0){
            queryLimit = "LIMIT " + displayCount + " ;";
        }
    }
    QString queryStr = queryBase + queryFilter1 + queryFilter2 + queryFilter3 + querySort + queryLimit;
//    qDebug()<<"queryFilterManage:"<<queryStr;
    emit sendQueryStr(queryStr);
}

//查询
void queryDialog::on_but_search_clicked()
{
    queryFilterManage(0);
}

//一线的车型选择管理
void queryDialog::on_checkBox_L1_clicked(bool checked)
{
    QStringList carType_l1;
    QStringList carType_l2;
    if(checked){
        carType_l1 = carType_L1;
        carType_l2 = carType_L2;
    }else{
        carType_l2 = carType_L2;
    }
    search_carTypeManage(carType_l1, carType_l2);
}

//二线的车型选择管理
void queryDialog::on_checkBox_L2_clicked(bool checked)
{
    QStringList carType_l1;
    QStringList carType_l2;
    if(checked){
        carType_l1 = carType_L1;
        carType_l2 = carType_L2;
    }else{
        carType_l1 = carType_L1;
    }
    search_carTypeManage(carType_l1, carType_l2);
}

//上一页
void queryDialog::on_but_previous_clicked()
{
    if(currentPage > 1){
        currentPage--;
        queryFilterManage(currentPage);
    }
}

//下一页
void queryDialog::on_but_next_clicked()
{
    if(currentPage < resultPage){
        currentPage++;
        queryFilterManage(currentPage);
    }
}

//首页
void queryDialog::on_but_first_clicked()
{
    if(currentPage > 1){
        currentPage = 1;
        queryFilterManage(currentPage);
    }
}

//尾页
void queryDialog::on_but_last_clicked()
{
    if(currentPage < resultPage){
        currentPage = resultPage;
        queryFilterManage(currentPage);
    }
}
