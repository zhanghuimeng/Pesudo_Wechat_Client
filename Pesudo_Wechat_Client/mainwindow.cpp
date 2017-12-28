#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    clientConnectionThread(new ClientConnectionThread())
{
    ui->setupUi(this);
    dialog = new LoginDialog(this);
    // refresh friend list
    connect(ui->refreshAction, SIGNAL(triggered(bool)), this, SLOT(slot_send_refresh_friends()));
    qRegisterMetaType<QMap<int,QString>>("QMap<int,QString>");
    connect(clientConnectionThread, SIGNAL(signal_friendlist_changed(QMap<int,QString>)), this,
            SLOT(slot_friendlist_changed(QMap<int,QString>)));
    // Send json from Child Thread
    connect(this, SIGNAL(signal_send(QJsonObject)), clientConnectionThread, SLOT(slot_send_json(QJsonObject)));
    clientConnectionThread->start();
    login();
}

void MainWindow::log(QString level, QString msg)
{
    qDebug() << level << QString(": MainWindow::%1").arg(msg) << endl;
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

void MainWindow::slot_friendlist_changed(QMap<int, QString> map)
{
    log("info", QString("slot_friendlist_changed(): map size = %1").arg(map.size()));
    foreach (int i, map.keys())
    {
        QString username = map[i];
        this->addFriend(i, username);
        this->friendMap.insert(i, username);
        log("info", QString("slot_friendlist_changed(): added friend, id=%1, username=%2").arg(i).arg(username));
    }
}

void MainWindow::slot_send_refresh_friends()
{
    /*
    action: "get_friends_list"
    username: "zhm_1"
    */
    QJsonObject jsonObject;
    jsonObject.insert("action", "get_friends_list");
    jsonObject.insert("username", this->username);
    emit signal_send(jsonObject);
    log("info", "slot_send_refresh_friends(): Sending request to server");
}

// add a friend to left
void MainWindow::addFriend(int id, QString username)
{
    ui->friendListWidget->addItem(username);
    this->chatboxMap.insert(id, new ChatBox());
}

MainWindow::~MainWindow()
{
    delete ui;
}
