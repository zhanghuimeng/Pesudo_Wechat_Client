#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "clientconnectionthread.h"

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

private slots:
    void slot_validate_user(QString username, QString password);

private:
    void log(QString level, QString msg);

    Ui::MainWindow *ui;
    LoginDialog* dialog;
    ClientConnectionThread* clientConnectionThread;
    QString username;
    QString password;
    QString info;
    QString error;
};

#endif // MAINWINDOW_H
