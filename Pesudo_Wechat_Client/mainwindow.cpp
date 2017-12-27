#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    clientConnectionThread(new ClientConnectionThread())
{
    ui->setupUi(this);
    dialog = new LoginDialog(this);
    clientConnectionThread->start();
    login();
}

void MainWindow::log(QString level, QString msg)
{
    qDebug() << "MainWindow: " << level << ": " << msg << endl;
}

void MainWindow::login()
{
    dialog->setWindowTitle(tr("登录"));
    connect(dialog, SIGNAL(signal_accepted_username(QString,QString)), this, SLOT(slot_validate_user(QString, QString)));
    connect(clientConnectionThread, SIGNAL(signal_user_validation(bool)), dialog, SLOT(slot_validation_result(bool)));
    dialog->show();
}

void MainWindow::slot_validate_user(QString username, QString password)
{
    this->username = username;
    this->password = password;
    info = QString("username = %1").arg(username);
    log("info", info);
    info = QString("password = %1").arg(password);
    log("info", info);
    info.clear();
    clientConnectionThread->slot_send_login(username, password);
}

MainWindow::~MainWindow()
{
    delete ui;
}
