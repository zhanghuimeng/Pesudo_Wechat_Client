#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "logindialog.h"
#include "clientconnectionthread.h"
#include "chatbox.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void login();
    ~MainWindow();

signals:
    void signal_username_result(bool correct);
    void signal_send(QJsonObject jsonObject);

public slots:
    void slot_validate_user(QString username, QString password);
    void slot_friendlist_changed(QMap<int, QString> map);
    void slot_send_refresh_friends();

protected:
    void addFriend(int id, QString username);

private:
    void log(QString level, QString msg);

    Ui::MainWindow *ui;
    LoginDialog* dialog;
    ClientConnectionThread* clientConnectionThread;
    QString username;
    QString password;
    QString info;
    QString error;

    QMap<int, QString> friendMap;
    QMap<int, ChatBox*> chatboxMap;
};

#endif // MAINWINDOW_H
