#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include <QUrl>
#include <QThread>
#include <QSet>
#include "user.h"

#define MAXLEN 1050000
#define PORT 2326

class ClientThread: public QThread
{
    Q_OBJECT
public:
    ClientThread();
    void run();

signals:
    void signal_user_validation(bool succeeded);
    void signal_friendlist_changed(QMap<int, QString>);
    void signal_received_text(QJsonObject jsonObject);  // send to MainWindow
    void signal_received_file(QDateTime time, QString sender, QString receiver, QString curFilePath);  // send to MainWindow

public slots:
    void slot_send_login(QString username, QString password);
    void slot_send_bytes(const char* bytes);
    void slot_send_json(QJsonObject jsonObject);
    void slot_send_file(int id, QDateTime time, QUrl fileUrl);  // send by MainWindow

protected:
    void parseReceived(const char* bytes);

private:
    void log(QString level, QString msg);
    QByteArray jsonToString(QJsonObject json);
    QJsonObject stringToJson(const char *bytes);

    char* buffer;
    int socketfd;  // 套接字描述符
    int port;
    struct sockaddr_in serverAddr;
    QString error;
    QString info;
    UserMap userMap;
    QMap<int, User*> idToUserMap;
    QString username;
    QString password;

    bool userValidated;
};

#endif // CLIENTCONNECTION_H
