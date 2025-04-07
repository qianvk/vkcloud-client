#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>

#include "userdata.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

    void AppendChatMsg(std::shared_ptr<TextChatData> msg);
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void SlotAppendMsg();
    void SlotAppendMessage(std::shared_ptr<UserInfo> user_info, std::shared_ptr<TextChatData> msg);

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATPAGE_H
