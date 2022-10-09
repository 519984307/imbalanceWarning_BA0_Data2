#ifndef PASSWORDPANEL_H
#define PASSWORDPANEL_H

#include <QDialog>

namespace Ui {
class PasswordPanel;
}

class PasswordPanel : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordPanel(QWidget *parent = 0);
    ~PasswordPanel();
    QString nextWin;

private slots:

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_0_clicked();

    void on_pushButton_delete_clicked();

    void on_but_close_clicked();

signals:
    void sendResult(bool,QString);

private:
    Ui::PasswordPanel *ui;
    int isFull;
    QString temp;
    void judge();
};

#endif // PASSWORDPANEL_H
