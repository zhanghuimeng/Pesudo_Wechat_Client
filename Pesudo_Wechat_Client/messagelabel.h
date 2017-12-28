#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QWidget>
#include <QLabel>

class MessageLabel : public QLabel
{
    Q_OBJECT

public:
    enum Type
    {
        SEND, RECEIVE
    };

    explicit MessageLabel(Type type, QString text, QWidget *parent = 0);
    ~MessageLabel();    

private:
    Type type;
};

#endif // MESSAGELABEL_H
