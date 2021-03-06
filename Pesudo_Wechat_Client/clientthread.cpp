#include "clientthread.h"
#include <QDebug>
#include <QFile>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

ClientThread::ClientThread(): QThread(), port(PORT), userValidated(false), buffer(new char[MAXLEN])
{   
}

QByteArray ClientThread::jsonToString(QJsonObject json)
{
    QJsonDocument doc(json);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);
    log("info", QString("jsonToString(): Converted QJsonObject to QByteArray: length=%1").arg(ba.size()));
    /*
    QJsonObject json2 = stringToJson(ba.data());
    log("info", QString("reconvert the last json to jsonobject, json=%1").arg(jsonToReadableString(json2).data()));
    log("info", QString("action = %1").arg(json2.find("action").value().toString()));
    */
    return ba;
}

QJsonObject ClientThread::stringToJson(const char *bytes)
{
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject json = doc.object();
    return json;
}

void ClientThread::log(QString level, QString msg)
{
    qDebug() << level << QString(": ClientConnectionThread::%1").arg(msg) << endl;
}

/**
 * @brief wait for server to send bunchs of bytes
 */
void ClientThread::run()
{
    log("info", "run(): Starting this thread");
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
    log("info", "run(): Created a socket file handle");

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
        buffer[n] = '\0';
        if (n == 0 || strlen(buffer) == 0)  // has received nothing
            continue;
        log("info", QString("run(): Received string from server: length=%1, content=%2").arg(n).arg(buffer));

        parseReceived(buffer);
        memset(buffer, 0, sizeof(char) * MAXLEN);
    }
    close(socketfd);
}

void ClientThread::parseReceived(const char *bytes)
{
    QJsonObject jsonRec = stringToJson(bytes);
    QString action = jsonRec.find("action").value().toString();
    log("info", QString("parseReceived(): data=%1").arg(bytes));
    log("info", QString("parseReceived(): action=%1").arg(action));

    /*
    action: "server_login_response"
    correct: "true/false"
    */
    if (action == "server_login_response")
    {
        log("info", "parseReceived(): Action: server login response");
        bool result = jsonRec.find("correct").value().toBool();
        if (!this->userValidated)
        {
            if (result)
            {
                emit signal_user_validation(true);
                this->userValidated = true;
                log("info", "parseReceived(): Login succeeded.");
            }
            else
            {
                emit signal_user_validation(false);
                log("info", "parseReceived(): Wrong username or password.");
            }
        }
        // else ignore
    }
    /*
    action: "send_friends_list_to_client"
    friends: [{username: "zhm_2"}, {username: "zhm_3"}]
    */
    else if (action == "send_friends_list_to_client")
    {
        log("info", "parseReceived(): Action: receive friends information");
        bool changed = false;
        QJsonArray friendArray = jsonRec.find("friends").value().toArray();
        // log("info", QString("parseReceived(): friend size=%1").arg(friendArray.size()));
        // log("info", QString("parseReceived(): username list size=%1").arg(userMap.getUsernameList().size()));
        QMap<int, QString> newFriendMap;
        foreach (const QJsonValue& value, friendArray)
        {
            QJsonObject obj = value.toObject();
            QString username = obj.find("username").value().toString();
            // log("info", QString("parseReceived(): friend username=%1").arg(username));
            // log("info", QString("parseReceived(): hasUser=%1").arg(userMap.hasUser(username)));
            if (!userMap.hasUser(username))
            {
                changed = true;
                User* user = new User(username);
                userMap.addUser(user);
                idToUserMap.insert(user->getId(), user);
                newFriendMap.insert(user->getId(), username);
                log("info", QString("parseReceived(): new friend username=%1, id=%2").arg(username).arg(user->getId()));
            }
        }
        if (changed)
            emit signal_friendlist_changed(newFriendMap);
    }
    /*
    action: "send_text_to_client"
    text: {text: "send some text blabla...", time: "2017/1/1:00:00:00", sendby: "zhm_1", sendto: "zhm_2"}
    */
    else if (action == "send_text_to_client")
    {
        log("info", QString("parseReceived(): received text message"));
        emit signal_received_text(jsonRec);
    }
    /*
    action: "send_file_to_client"
    file: {filename: "a.png", content: "用base64压缩之后的文件", time: "2017/1/1:00:00:00", sendby: "zhm_1", sendto: "zhm_2"}
    */
    else if (action == "send_file_to_client")
    {
        // save file, send to ui
        // ~/Downloads/
        QJsonObject fileObject = jsonRec.find("file").value().toObject();
        QString filename = fileObject.find("filename").value().toString();
        QDateTime time = QDateTime::fromTime_t(qint64(fileObject.find("time").value().toInt()));
        QString sender = fileObject.find("sendby").value().toString();
        QString receiver = fileObject.find("sendto").value().toString();

        QString rawContent = fileObject.find("content").value().toString();
        QByteArray encrypted = QByteArray::fromBase64(rawContent.toLocal8Bit());  // TOOD: is this right?  ok.
        QByteArray original = qUncompress(encrypted);
        QString curFilePath = QString("/home/zhanghuimeng/Downloads/%1_%2").arg(time.toTime_t()).arg(filename);
        QFile file(curFilePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            log("error", QString("parseReceived(): cannot open file %1 to write").arg(curFilePath));
            return;
        }

        file.write(original.data(), original.size());
        file.close();

        emit signal_received_file(time, sender, receiver, curFilePath);
    }
}

/**
 * @brief send a bunch of bytes
 * @param bytes
 */
void ClientThread::slot_send_bytes(const char *bytes)
{
    if (send(socketfd, bytes, strlen(bytes), 0) < 0)
    {
        log("error", "slot_send_bytes(): Cannot send data to server");
        return;
    }
    log("info", QString("slot_send_bytes(): Send data to server, length=%1").arg(strlen(bytes)));
}

void ClientThread::slot_send_json(QJsonObject jsonObject)
{
    char* data = jsonToString(jsonObject).data();
    slot_send_bytes(data);
}

/*
action: "client_login"
username: "zhm_x"
password: "123456"
*/
void ClientThread::slot_send_login(QString username, QString password)
{
    QJsonObject json;
    json.insert("action", QJsonValue("client_login"));
    json.insert("username", QJsonValue(username));
    json.insert("password", QJsonValue(password));
    slot_send_json(json);

    log("info", "slot_send_login(): Send login info to server");
    this->username = username;
    this->password = password;
    // not really right, but according to logic should be right
}

/*
action: "send_file_to_server"
file: {filename: "a.png", content: "用base64压缩之后的文件", time: "2017/1/1:00:00:00", sendby: "zhm_1", sendto: "zhm_2"}
*/
void ClientThread::slot_send_file(int id, QDateTime time, QUrl fileUrl)
{
    if (!idToUserMap.contains(id) || idToUserMap.find(id).value() == NULL)
    {
        log("error", QString("slot_send_file(): cannot find receiver id %1").arg(id));
        return;
    }

    QString receiver = idToUserMap.find(id).value()->getUsername();
    log("info", QString("slot_send_file(): file receiver name is %1").arg(receiver));
    QFile file(fileUrl.path());
    if (!file.open(QIODevice::ReadOnly))
    {
        log("error", QString("slot_send_file(): cannot open file %1").arg(fileUrl.path()));
        return;
    }

    // compress and base64 (to ASCII)
    QByteArray blob = file.readAll();
    QByteArray encrypted = qCompress(blob);
    QString base64ed = encrypted.toBase64().toStdString().c_str();
    log("info", QString("slot_send_file(): file original size=%1, compressed size=%2, base84 size=%3")
        .arg(blob.size()).arg(encrypted.size()).arg(base64ed.size()));

    QJsonObject jsonObject;
    QJsonObject fileObject;
    fileObject.insert("filename", QJsonValue(fileUrl.fileName()));
    fileObject.insert("content", QJsonValue(base64ed));
    fileObject.insert("time", QJsonValue(qint64(time.toTime_t())));
    fileObject.insert("sendby", QJsonValue(this->username));
    fileObject.insert("sendto", QJsonValue(receiver));

    jsonObject.insert("file", QJsonValue(fileObject));
    jsonObject.insert("action", "send_file_to_server");

    this->slot_send_json(jsonObject);
}
