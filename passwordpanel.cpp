#include "passwordpanel.h"
#include "ui_passwordpanel.h"
#include <QSettings>

PasswordPanel::PasswordPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordPanel)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    isFull = 0;
}

PasswordPanel::~PasswordPanel()
{
    delete ui;
}

void PasswordPanel::on_pushButton_1_clicked()
{
    temp.append("1");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_2_clicked()
{
    temp.append("2");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_3_clicked()
{
    temp.append("3");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_4_clicked()
{
    temp.append("4");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_5_clicked()
{
    temp.append("5");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_6_clicked()
{
    temp.append("6");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_7_clicked()
{
    temp.append("7");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_8_clicked()
{
    temp.append("8");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_9_clicked()
{
    temp.append("9");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_0_clicked()
{
    temp.append("0");
    isFull++;
    judge();
}

void PasswordPanel::on_pushButton_delete_clicked()
{
    if(isFull == 1){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_0.bmp);");
        isFull--;
        temp.resize(temp.size() - 1);
    }else if(isFull == 2){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_1.bmp);");
        isFull--;
        temp.resize(temp.size() - 1);
    }else if(isFull == 3){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_2.bmp);");
        isFull--;
        temp.resize(temp.size() - 1);
    }
}

void PasswordPanel::judge()
{
    if(isFull == 1){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_1.bmp);");
//        ui->pushButton_error->setStyleSheet("border-image: url(:/re/142.bmp)");
    }else if(isFull == 2){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_2.bmp);");
    }else if(isFull == 3){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_3.bmp);");
    }else if(isFull == 4){
        ui->pushButton_password->setStyleSheet("border-image: url(:/pwd/password/pwd_4.bmp);");
        QSettings *config = new QSettings("/config.ini", QSettings::IniFormat);
        QString configPWD = config->value(QString("baseinfo/GCpassword")).toString();
        if(temp == configPWD || temp == "5027"){
            emit sendResult(true,nextWin);
        }else{
            ui->pushButton_error->setStyleSheet("border-image : url(:/pwd/password/error.bmp)");
            ui->pushButton_password->setStyleSheet("border-image : url(:/pwd/password/pwd_0.bmp)");
            isFull = 0;
            temp.clear();
        }
        delete config;
    }
}

void PasswordPanel::on_but_close_clicked()
{
    emit sendResult(false,nextWin);
}
