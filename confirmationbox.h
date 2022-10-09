#ifndef CONFIRMATIONBOX_H
#define CONFIRMATIONBOX_H

#include <QDialog>

namespace Ui {
class confirmationBox;
}

class confirmationBox : public QDialog
{
    Q_OBJECT

public:
    explicit confirmationBox(QWidget *parent = 0);
    ~confirmationBox();
    QString warningType;

private slots:
    void on_confirmation_clicked();

    void on_cancellation_clicked();

signals:
    void sendSelectedResult(bool);
private:
    Ui::confirmationBox *ui;
};

#endif // CONFIRMATIONBOX_H
