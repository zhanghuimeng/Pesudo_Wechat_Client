#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->errorLabel->hide();
    connect(ui->usernameLineEdit, SIGNAL(textEdited(QString)), ui->errorLabel, SLOT(hide()));
    connect(ui->passwordLineEdit, SIGNAL(textEdited(QString)), ui->errorLabel, SLOT(hide()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::slot_validation_result(bool result)
{
    if (result)
    {
        ((QWidget*) parent())->show();
        this->destroy();
    }
    else
    {
        ui->errorLabel->show();
        ui->passwordLineEdit->clear();
    }
}

void LoginDialog::on_buttonBox_accepted()
{
    emit signal_accepted_username(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}
