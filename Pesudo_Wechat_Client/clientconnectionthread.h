#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>
#include <QSet>
#include "user.h"

#define MAXLEN 1050000
#define PORT 3333

class ClientConnectionThread: public QThread
{
    Q_OBJECT
public:
    ClientConnectionThread();
    void run();

signals:
    void signal_user_validation(bool succeeded);
    void signal_friendlist_changed(QMap<int, QString>);

public slots:
    void slot_send_login(QString username, QString password);
    void slot_send_bytes(const char* bytes);
    void slot_send_json(QJsonObject jsonObject);

protected:
    void parseReceived(const char* bytes);

private:
    void log(QString level, QString msg);
    QByteArray jsonToString(QJsonObject json);
    QByteArray jsonToReadableString(QJsonObject json);
    QJsonObject stringToJson(const char *bytes);

    char* buffer;
    int socketfd;  // 套接字描述符
    int port;
    struct sockaddr_in serverAddr;
    QString error;
    QString info;
    UserMap userMap;

    bool userValidated;
};

#endif // CLIENTCONNECTION_H
