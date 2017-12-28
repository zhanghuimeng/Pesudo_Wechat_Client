#include "chatbox.h"
#include "ui_chatbox.h"
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
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

void ChatBox::log(QString level, QString msg)
{
    qDebug() << level << QString(": ChatBox::%1").arg(msg) << endl;
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
    QDateTime time = QDateTime::currentDateTime();
    ui->messageListWidget->slot_add_right(time,
                QObject::tr(QString("上传文件\n%1").arg(fileUrl.toString()).toStdString().c_str()));
    emit signal_send_file(id, time, fileUrl);
}

void ChatBox::slot_clicked_send()
{
    log("info", "slot_clicked_send()");
    if (ui->textEdit->toPlainText().size() == 0)
        ui->emptyLabel->show();
    QString text = ui->textEdit->toPlainText();
    QDateTime time = QDateTime::currentDateTime();
    ui->messageListWidget->slot_add_right(time, text);
    ui->textEdit->clear();
    emit signal_send_text(id, time, text);
}

/**
 * @brief receive text from server
 * @param text
 * @param time
 */
void ChatBox::slot_received_text(QString text, QDateTime time)
{
    log("info", QString("slot_received_text(): time=%1, text=%2").arg(time.toString()).arg(text));
    ui->messageListWidget->slot_add_left(time, text);
}

/**
 * @brief ChatBox::slot_received_file
 */
void ChatBox::slot_received_file()
{
    // TODO
}

ChatBox::~ChatBox()
{
    delete ui;
}
