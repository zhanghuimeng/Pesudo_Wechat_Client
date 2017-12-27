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

#define MAXLEN 1050000

class ClientConnectionThread: public QThread
{
public:
    ClientConnectionThread();
    void run();

public slots:
    void slot_send_login(QString username, QString password);
    void slot_send_bytes(const char* bytes);

private:
    void log(QString level, QString msg);
    QByteArray jsonToString(QJsonObject json);

    char buffer[MAXLEN];
    int socketfd;  // 套接字描述符
    int port;
    struct sockaddr_in serverAddr;
    QString error;
    QString info;
};

#endif // CLIENTCONNECTION_H
