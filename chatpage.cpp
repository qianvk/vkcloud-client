#include "chatpage.h"
#include "ui_chatpage.h"

#include <QStyleOption>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <spdlog/spdlog.h>

#include "usermanager.h"
#include "chatitembase.h"
#include "textbubble.h"
#include "tcpmanager.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    _user_info = std::shared_ptr<UserInfo>(new UserInfo(0, "qinwy", "xxx"));
    connect(ui->text_edit, &MessageTextEdit::send, this, &ChatPage::SlotAppendMsg);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::Instance()->GetUserInfo();
    ChatRole role;
    //todo... 添加聊天显示
    if (msg->is_self_) {
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->SetUserName(self_info->_name);
        pChatItem->SetUserIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_history_wid->AppendChatItem(pChatItem);
    }
    else {
        auto friend_info = UserMgr::Instance()->GetFriendById(msg->_from_uid);
        if (friend_info == nullptr)
            return;

        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(friend_info->_name);
        pChatItem->SetUserIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_history_wid->AppendChatItem(pChatItem);
    }
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    ui->chat_history_wid->ClearChatItem();
    _user_info = user_info;
    ui->title_lb->setText(user_info->_nick);
    for (const auto& msg : user_info->_chat_msgs)
        AppendChatMsg(msg);
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ChatPage::SlotAppendMsg()
{
    if (_user_info == nullptr) {
        SPDLOG_ERROR("friend_info is empty");
        return;
    }

    auto user_info = UserMgr::Instance()->GetUserInfo();
    auto pTextEdit = ui->text_edit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 4096){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(userName);
        pChatItem->SetUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
            if(txt_size + msgList[i].content.length()> 1024){
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpMgr::Instance()->sig_send_data(RequestId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["type"] = 1;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>("", obj["content"].toString(),
                                                          user_info->_uid, _user_info->_uid);
            _user_info->_chat_msgs.push_back(txt_msg);
            // emit sig_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            // pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {

        }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->SetWidget(pBubble);
            ui->chat_history_wid->AppendChatItem(pChatItem);
        }

    }

    //发送给服务器
    textObj["msgId"] = static_cast<int>(MessageId::ID_TEXT_CHAT_MSG_REQ);
    textObj["contents"] = textArray;
    textObj["relateId"] = _user_info->relate_id_;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    SPDLOG_INFO("Send message: {}", jsonData.toStdString());
    emit WebSocketClient::Instance()->SigSendText(QString::fromUtf8(jsonData));
#if 0
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpMgr::Instance()->sig_send_data(RequestId::ID_TEXT_CHAT_MSG_REQ, jsonData);
#endif

}
