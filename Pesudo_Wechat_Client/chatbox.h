#ifndef CHATBOX_H
#define CHATBOX_H

#include <QWidget>

namespace Ui {
class ChatBox;
}

class ChatBox : public QWidget
{
    Q_OBJECT

public:
    explicit ChatBox(QWidget *parent = 0);
    ~ChatBox();

signals:
    void signal_send_text(QString text);
    void signal_send_file(QString filename);

public slots:
    void slot_clicked_send();  // send message
    void slot_clicked_file();  // send file

private:
    Ui::ChatBox *ui;
};

#endif // CHATBOX_H
