#include "clientconnectionthread.h"
#include <QDebug>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

ClientConnectionThread::ClientConnectionThread(): QThread(), port(6666), userValidated(false), buffer(new char[MAXLEN])
{   
}

QByteArray ClientConnectionThread::jsonToString(QJsonObject json)
{
    QJsonDocument doc(json);
    QByteArray ba = doc.toBinaryData();
    return ba;
}

void ClientConnectionThread::log(QString level, QString msg)
{
    qDebug() << "ClientConnectionThread: " << level << ": " << msg << endl;
}

/**
 * @brief wait for server to send bunchs of bytes
 */
void ClientConnectionThread::run()
{
    log("info", "Starting this thread");
    // Use linux socket to establish the server
    /* 该函数包含三个参数:
     * domain参数用于指定所创建套接字的协议类型。通常选用AF_INET，表示使用IPv4的TCP/IP协议；如果只在本机内进行进程间通信，则可以使用AF_UNIX。
     * 参数type用来指定套接字的类型，SOCK_STREAM用于创建一个TCP流的套接字，SOCK_DGRAM用于创建UDP数据报套接字。
     * 参数protocol通常取0
     */
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        error = "Cannot create a socket file handle";
        log("error", error);
        error.clear();
        return;
    }
    info = "Created a socket file handle";
    log("info", info);
    info.clear();

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0)
    {
        error = QString("inet_pton error for 127.0.0.1");
        log("error", error);
        error.clear();
        return;
    }
    info = QString("no inet_pton error for 127.0.0.1");
    log("info", info);
    error.clear();

    if (::connect(socketfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    {
        error = "Cannot connect to server";
        log("error", error);
        error.clear();
        return;
    }
    info = "Connected to server";
    log("info", info);
    info.clear();

    // jumping into the major loop
    while (true)
    {
        int n = recv(socketfd, buffer, MAXLEN, 0);
        if (n < 0)
        {
            error = "Receiving error";
            log("error", error);
            error.clear();
            continue;
        }
        if (strlen(buffer) == 0)  // has received nothing
        {
            continue;
        }
        info = QString("Received string from server: length=%s").arg(n);
        log("info", info);
        info.clear();

        parseReceived(buffer, n);
    }
    close(socketfd);
}

void ClientConnectionThread::parseReceived(const char *bytes, int length)
{
    QJsonDocument doc = QJsonDocument::fromRawData(bytes, length);
    QJsonObject jsonRec = doc.object();
    QString action = jsonRec.find("action").value().toString();

    /*
    action: "server_login_response"
    correct: "true/false"
    */
    if (action == "server_login_response")
    {
        log("info", "Action: server login response");
        bool result = jsonRec.find("correct").value().toBool();
        if (!this->userValidated)
        {
            if (result)
            {
                emit signal_user_validation(true);
                this->userValidated = true;
                log("info", "Login succeeded.");
            }
            else
            {
                emit signal_user_validation(false);
                log("info", "Wrong username or password.");
            }
        }
        // else ignore
    }
}

/**
 * @brief send a bunch of bytes
 * @param bytes
 */
void ClientConnectionThread::slot_send_bytes(const char *bytes)
{
    if (send(socketfd, bytes, strlen(bytes), 0) < 0)
    {
        error = "Cannot send data to server";
        log("error", error);
        error.clear();
    }
    info = QString("Send data to server, length=%1").arg(strlen(bytes));
    log("info", info);
    info.clear();
}

/*
action: "client_login"
username: "zhm_x"
password: "123456"
*/
void ClientConnectionThread::slot_send_login(QString username, QString password)
{
    QJsonObject json;
    json.insert("action", QJsonValue("client_login"));
    json.insert("username", QJsonValue(username));
    json.insert("password", QJsonValue(password));
    char* data = jsonToString(json).data();
    slot_send_bytes(data);

    info = QString("Send login info to server: length=%1").arg(data);
    log("info", info);
    info.clear();
}
