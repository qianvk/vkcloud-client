#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>

#include "singleton.h"
#include "global.h"
#include "userdata.h"

class QWebSocket;
class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    friend class Singleton<TcpMgr>;
    Q_OBJECT
public:
    ~ TcpMgr();
private:
    TcpMgr();
    void initHandlers();
    void handleMsg(RequestId id, int len, QByteArray data);
    QSslSocket ssl_socket_;
    QString host_;
    QString token_;
    quint32 user_id_;
    quint16 port_;
    QByteArray buffer_;
    bool is_rcv_pending_;
    quint16 message_id_;
    quint16 message_len_;
    QMap<RequestId, std::function<void(RequestId id, int len, QByteArray data)>> handlers_;
public slots:
    void slot_connect_chat(const ServerInfo& server_info);
    void slot_send_data(RequestId reqId, QByteArray data);
signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(RequestId reqId, QByteArray data);
    void sig_swich_chatdlg();
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(StatusCode);
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);

};

class WebSocketClient : public QObject, public Singleton<WebSocketClient>
{
    friend class Singleton<WebSocketClient>;
    Q_OBJECT

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(MessageId msgId, QByteArray data);
    void sig_swich_chatdlg();
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(StatusCode);
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
    void SigSendText(const QString& text);
    void SigAppendMessage(std::shared_ptr<UserInfo> user_info, std::shared_ptr<TextChatData> msg);

public slots:
    void SlotConnectChat(const ServerInfo& server_info);
    void SlotSendData(MessageId msgId, QByteArray data);
    void SlotSendBinary(const QByteArray& data);
    void SlotSendText(const QString& text);

private slots:
    void onConnected();

    void onTextMessageReceived(const QString &message);
    void onDisconnected();

    void onSslErrors(const QList<QSslError> &errors);

private:
    WebSocketClient();

private:
    void InitHandlers();

    QWebSocket *m_webSocket;
    QString host_;
    QString token_;
    quint32 user_id_;
    quint16 port_;
    QByteArray buffer_;
    bool is_rcv_pending_;
    quint16 message_id_;
    quint16 message_len_;
    std::array<std::function<void(const QJsonObject&)>, static_cast<int>(MessageId::kMessageIdNum)> handlers_;
};

#endif // TCPMANAGER_H
