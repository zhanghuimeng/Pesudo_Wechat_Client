#include "messagelabel.h"

MessageLabel::MessageLabel(Type type, QString text, QWidget *parent) :
    type(type), QLabel(text, parent)
{
    if (type == Type::SEND)
        this->setStyleSheet("background: #62b900;");
    else
        this->setStyleSheet("background: white;");

    this->setMinimumHeight(30);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    // this->setMaximumHeight(30);
    this->setMinimumWidth(80);
    this->setMaximumWidth(200);
}

MessageLabel::~MessageLabel()
{

}
