#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>

class QScrollArea;

class ChatView : public QWidget
{
    Q_OBJECT
public:
    explicit ChatView(QWidget *parent = nullptr);

    void AppendChatItem(QWidget *item);
    void ClearChatItem();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool is_loading_bottom_;
    QScrollArea *p_chat_area_;

public slots:
    void SlotVScrollBarMoved(int min, int max);

signals:
};

#endif // CHATVIEW_H
