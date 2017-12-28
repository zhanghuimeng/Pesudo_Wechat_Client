#include "messagelistwidget.h"
#include "ui_messagelistwidget.h"
#include "messagelabel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

MessageListWidget::MessageListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageListWidget)
{
    ui->setupUi(this);
    this->mainVLayout = new QVBoxLayout();
    this->mainVLayout->setSpacing(10);
    this->mainVLayout->setAlignment(Qt::AlignTop);
    QFrame* inner = new QFrame(ui->scrollArea);
    inner->setLayout(mainVLayout);
    ui->scrollArea->setWidget(inner);
    ui->scrollArea->setWidgetResizable(true);
    // ui->scrollArea->setWidget(mainVLayout->widget());
}

void MessageListWidget::slot_add_left(QString text)
{
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(new MessageLabel(MessageLabel::SEND, text, this));
    hLayout->addStretch();
    mainVLayout->addLayout(hLayout);
}

void MessageListWidget::slot_add_right(QString text)
{
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addStretch();
    hLayout->addWidget(new MessageLabel(MessageLabel::SEND, text, this));
    mainVLayout->addLayout(hLayout);
}

MessageListWidget::~MessageListWidget()
{
    delete ui;
}
