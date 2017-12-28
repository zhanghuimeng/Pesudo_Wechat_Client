#include "user.h"

int User::userCount = 0;

User::User(QString username): username(username)
{
    this->id = userCount++;
}
