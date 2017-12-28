#ifndef USER_H
#define USER_H

#include <QMap>

class User
{
public:
    static int userCount;

    User(QString username);

    QString getUsername() const
    {
        return username;
    }

    int getId() const
    {
        return id;
    }

private:
    QString username;
    int id;
};

class UserMap
{
public:
    UserMap()
    {

    }

    User* findByUsername(QString username)
    {
        return userMap.find(username).value();
    }

    bool hasUser(QString username)
    {
        QList<QString> list = this->getUsernameList();
        for (int i = 0; i < list.size(); i++)
        {
            if (list[i] == username)
                return true;
        }
        return false;
    }

    bool addUser(User* user)
    {
        if (user == NULL)
            return false;
        if (userMap.contains(user->getUsername()))
            return false;
        userMap.insert(user->getUsername(), user);
        return true;
    }

    QList<QString> getUsernameList()
    {
        return userMap.keys();
    }

private:
    QMap<QString, User*> userMap;  // username -> user
};

#endif // USER_H
