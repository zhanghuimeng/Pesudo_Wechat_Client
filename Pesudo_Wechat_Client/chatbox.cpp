#include "chatbox.h"
#include "ui_chatbox.h"
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>

ChatBox::ChatBox(int id, QString username, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatBox),
    id(id),
    username(username)
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
    QUrl fileUrl = QFileDialog::getOpenFileUrl(this, tr("上传文件"), QUrl("/home/"), "All Files (size < 1MB)");
    QFileInfo fileInfo(fileUrl.path());
    if (fileInfo.size() > 1024*1024)  // 1MB
    {
        QMessageBox::critical(this, QObject::tr("错误"),
                              QObject::tr(QString("文件大小为%1MB，大于1MB").arg(fileInfo.size() / 1024.0*1024.0).toStdString().c_str()),
                              QMessageBox::Ok);
        return;
    }
    ui->messageListWidget->slot_add_right(
                QObject::tr(QString("上传文件\n%1").arg(fileUrl.toString()).toStdString().c_str()));
    emit signal_send_file(id, QDateTime::currentDateTime(), fileUrl);
}

void ChatBox::slot_clicked_send()
{
    if (ui->textEdit->toPlainText().isEmpty())
        ui->emptyLabel->show();
    QString text = ui->textEdit->toPlainText();
    ui->messageListWidget->slot_add_right(text);
    ui->textEdit->clear();
    emit signal_send_text(id, QDateTime::currentDateTime(), text);
}

ChatBox::~ChatBox()
{
    delete ui;
}
