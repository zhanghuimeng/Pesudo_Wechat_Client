#ifndef CHATBOX_H
#define CHATBOX_H

#include <QWidget>
#include <QDateTime>
#include <QUrl>

namespace Ui {
class ChatBox;
}

class ChatBox : public QWidget
{
    Q_OBJECT

public:
    explicit ChatBox(int id, QString username, QWidget *parent = 0);
    ~ChatBox();

signals:
    void signal_send_text(int id, QDateTime time, QString text);
    void signal_send_file(int id, QDateTime time, QUrl fileUrl);

public slots:
    void slot_clicked_send();  // send message
    void slot_clicked_file();  // send file

private:
    Ui::ChatBox *ui;
    int id;
    QString username;
};

#endif // CHATBOX_H
