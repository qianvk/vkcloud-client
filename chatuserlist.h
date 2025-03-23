#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>
#include <QTimer>

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void SigLoadingChatUser();

private slots:
    void SlotPerformSmoothScroll();

private:
    QPoint accumulated_delta_;
    QTimer scroll_timer_;
};

#endif // CHATUSERLIST_H
