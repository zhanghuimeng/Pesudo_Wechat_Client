#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

signals:
    void signal_accepted_username(QString username, QString password);

public slots:
    void slot_validation_result(bool result);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
