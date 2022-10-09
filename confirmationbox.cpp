#include "confirmationbox.h"
#include "ui_confirmationbox.h"

confirmationBox::confirmationBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::confirmationBox)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    ui->confirmation->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
    ui->cancellation->setFocusPolicy(Qt::NoFocus);//不显示点击效果，不影响点击事件
}

confirmationBox::~confirmationBox()
{
    delete ui;
}

//确认
void confirmationBox::on_confirmation_clicked()
{
    emit sendSelectedResult(true);
}

//取消
void confirmationBox::on_cancellation_clicked()
{
    emit sendSelectedResult(false);
}
