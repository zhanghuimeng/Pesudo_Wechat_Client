#ifndef MESSAGELISTWIDGET_H
#define MESSAGELISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QDateTime>

namespace Ui {
class MessageListWidget;
}

class MessageListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageListWidget(QWidget *parent = 0);
    ~MessageListWidget();

public slots:
    void slot_add_left(QDateTime time, QString text);
    void slot_add_right(QDateTime time, QString text);

private:
    Ui::MessageListWidget *ui;
    QVBoxLayout* mainVLayout;
};

#endif // MESSAGELISTWIDGET_H
