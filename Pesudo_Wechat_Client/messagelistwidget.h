#ifndef MESSAGELISTWIDGET_H
#define MESSAGELISTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>

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
    void slot_add_left(QString text);
    void slot_add_right(QString right);

private:
    Ui::MessageListWidget *ui;
    QHBoxLayout* hLayout;
};

#endif // MESSAGELISTWIDGET_H