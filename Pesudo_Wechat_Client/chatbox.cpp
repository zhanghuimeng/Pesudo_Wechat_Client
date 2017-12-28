#include "chatbox.h"
#include "ui_chatbox.h"
#include <QFileDialog>

ChatBox::ChatBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatBox)
{
    ui->setupUi(this);
    ui->emptyLabel->hide();
    // send to Client Thread
    connect(ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(slot_clicked_send()));
    connect(ui->uploadButton, SIGNAL(clicked(bool)), this, SLOT(slot_clicked_file()));
    // hide empty label
    connect(ui->textEdit, SIGNAL(textChanged()), ui->emptyLabel, SLOT(hide()));
}

void ChatBox::slot_clicked_file()
{
    QString file = QFileDialog::getOpenFileName("/home/", "All Files (size < 1MB)", this);
    ui->messageListWidget->slot_add_right(tr(QString("上传文件\n") + file));
    emit signal_send_file(file);
}

void ChatBox::slot_clicked_send()
{
    if (ui->textEdit->toPlainText().isEmpty())
        ui->emptyLabel->show();
    QString text = ui->textEdit->toPlainText();
    ui->messageListWidget->slot_add_right(text);
    ui->textEdit->clear();
    emit signal_send_text(text);
}

ChatBox::~ChatBox()
{
    delete ui;
}
