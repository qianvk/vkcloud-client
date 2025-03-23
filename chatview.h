#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>

class QScrollArea;

class ChatView : public QWidget
{
    Q_OBJECT
public:
    explicit ChatView(QWidget *parent = nullptr);

private:
    bool is_loading_bottom_;
    QScrollArea *p_chat_area_;

signals:
};

#endif // CHATVIEW_H
