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
    Q_OBJECT
public:
    ClientConnectionThread();
    void run();

signals:
    void signal_user_validation(bool succeeded);

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

    bool userValidated;
};

#endif // CLIENTCONNECTION_H
