#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    clientConnectionThread(new ClientThread()), curId(-1)
{
    ui->setupUi(this);
    dialog = new LoginDialog(this);

    // refresh friend list
    connect(ui->refreshAction, SIGNAL(triggered(bool)), this, SLOT(slot_send_refresh_friends()));
    qRegisterMetaType<QMap<int,QString>>("QMap<int,QString>");
    connect(clientConnectionThread, SIGNAL(signal_friendlist_changed(QMap<int,QString>)), this,
            SLOT(slot_friendlist_changed(QMap<int,QString>)));
    // change talker
    connect(ui->friendListWidget, SIGNAL(currentTextChanged(QString)), this, SLOT(slot_change_talker(QString)));
    // Send json from Child Thread
    connect(this, SIGNAL(signal_send(QJsonObject)), clientConnectionThread, SLOT(slot_send_json(QJsonObject)));
    // receive text message
    connect(clientConnectionThread, SIGNAL(signal_received_text(QJsonObject)), this,
            SLOT(slot_received_text(QJsonObject)));

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

/**
 * @brief Connected with friendListWidget::currentTextChanged(QString)
 * @param username
 */
void MainWindow::slot_change_talker(QString username)
{
    int id = -1;
    QList<int> idList = this->friendMap.keys();
    for (int i = 0; i < idList.size(); i++)
    {
        if (friendMap.find(i).value() == username)
        {
            id = i;
            break;
        }
    }
    if (id == -1)
    {
        log("info", "slot_change_talker(): cannot find clicked item");
        return;
    }
    log("info", QString("slot_change_talker(): clicked item id=%1").arg(id));
    // hide old box
    if (curId != -1)
        chatboxMap.find(curId).value()->hide();

    curId = id;
    chatboxMap.find(curId).value()->show();
}

/*
action: "send_text_to_server"
text: {text: "send some text blabla...", time: "2017/1/1:00:00:00", sendby: "zhm_1", sendto: "zhm_2"}
*/
void MainWindow::slot_send_text(int id, QDateTime time, QString text)
{
    log("info", "slot_send_text(): I send text: " + text);
    QJsonObject jsonObject;
    QJsonObject textObject;
    textObject.insert("text", QJsonValue(text));
    textObject.insert("time", QJsonValue(qint64(time.toTime_t())));
    textObject.insert("sendby", this->username);
    textObject.insert("sendto", friendMap.find(id).value());
    jsonObject.insert("action", QJsonValue("send_text_to_server"));
    jsonObject.insert("text", QJsonValue(textObject));
    emit signal_send(jsonObject);
}

void MainWindow::slot_send_file(int id, QDateTime time, QUrl fileUrl)
{
    log("info", "slot_send_file(): I send file: " + fileUrl.toString());
}

// received text message from server
/*
action: "send_text_to_client"
text: {text: "send some text blabla...", time: "2017/1/1:00:00:00", sendby: "zhm_1", sendto: "zhm_2"}
*/
void MainWindow::slot_received_text(QJsonObject jsonObject)
{
    QJsonObject textObject = jsonObject.find("text").value().toObject();
    QString text = textObject.find("text").value().toString();
    QDateTime time = QDateTime::fromTime_t(uint(textObject.find("time").value().toInt()));
    QString sender = textObject.find("sendby").value().toString();
    QString sendto = textObject.find("sendto").value().toString();
    if (sendto != username)
    {
        log("error", QString("slot_received_text(): The receiver is not %1").arg(sendto));
        return;
    }
    int senderId = -1;
    QList<int> list = friendMap.keys();
    for (int i = 0; i < list.size(); i++)
    {
        if (friendMap.find(list[i]).value() == sender)
        {
            senderId = list[i];
            break;
        }
    }
    if (senderId == -1)
    {
        log("error", QString("slot_received_text(): Cannot find sender %1").arg(sender));
        return;
    }
    log("info", QString("slot_received_text(): The id of sender %1 is %2").arg(sender).arg(senderId));

    chatboxMap.find(senderId).value()->slot_received_text(text, time);
}

// add a friend to left
void MainWindow::addFriend(int id, QString username)
{
    ui->friendListWidget->addItem(username);
    ChatBox* chatBox = new ChatBox(id, username);
    this->chatboxMap.insert(id, chatBox);
    // ChatBox signal to send text
    connect(chatBox, SIGNAL(signal_send_file(int,QDateTime,QUrl)), this, SLOT(slot_send_file(int,QDateTime,QUrl)));
    connect(chatBox, SIGNAL(signal_send_text(int,QDateTime,QString)), this, SLOT(slot_send_text(int,QDateTime,QString)));
    // hide and add to layout
    chatBox->hide();
    ui->verticalLayout->addWidget(chatBox);
}

MainWindow::~MainWindow()
{
    delete ui;
}
