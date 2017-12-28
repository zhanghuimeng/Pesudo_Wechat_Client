#include "messagelistwidget.h"
#include "ui_messagelistwidget.h"
#include <QVBoxLayout>
#include <QLabel>

MessageListWidget::MessageListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageListWidget)
{
    ui->setupUi(this);
    this->hLayout = new QHBoxLayout(this);
}

void MessageListWidget::slot_add_left(QString text)
{
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(new QLabel(text, this));
    vLayout->addStretch();
    hLayout->addLayout(vLayout);
}

void MessageListWidget::slot_add_right(QString text)
{
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addWidget(new QLabel(text, this));
    hLayout->addLayout(vLayout);
}

MessageListWidget::~MessageListWidget()
{
    delete ui;
}
