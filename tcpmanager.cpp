#include "tcpmanager.h"

#include <QAbstractSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QtWebSockets/QWebSocketProtocol>
#include <QtWebSockets/QtWebSockets>

#include <spdlog/spdlog.h>

#include "usermanager.h"

#ifdef Q_OS_UNIX
#include <sys/socket.h>
#elif defined(Q_OS_WIN)
#include <winsock2.h>
#endif

TcpMgr::TcpMgr():host_(""),port_(0),is_rcv_pending_(false),message_id_(0),message_len_(0)
{
    int socketDescriptor = ssl_socket_.socketDescriptor();
    if (socketDescriptor != -1) {
#ifdef Q_OS_UNIX
        int enableKeepAlive = 1;
        if (setsockopt(socketDescriptor, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive)) == -1) {
            // Handle error
        }
#elif defined(Q_OS_WIN)
        BOOL enableKeepAlive = TRUE;
        if (setsockopt(socketDescriptor, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&enableKeepAlive), sizeof(enableKeepAlive)) == SOCKET_ERROR) {
            // Handle error
        }
#endif
    }

    QObject::connect(&ssl_socket_, &QSslSocket::connected, [&]() {
        SPDLOG_INFO("Connected to server!");
        // 连接建立后发送消息
        // emit sig_con_success(true);
    });
    
    QObject::connect(&ssl_socket_, &QSslSocket::encrypted, [&]() {
        SPDLOG_INFO("Connection encrypted!");
        emit sig_con_success(true);
        QJsonObject json_obj;
        json_obj["id"] = static_cast<qint64>(user_id_);
        json_obj["token"] = token_;

        QJsonDocument doc(json_obj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        //发送tcp请求给chat server
        emit TcpMgr::Instance()->sig_send_data(RequestId::ID_CHAT_LOGIN, jsonData);
    });

    QObject::connect(&ssl_socket_, &QSslSocket::readyRead, [&]() {
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        buffer_.append(ssl_socket_.readAll());

        SPDLOG_INFO("Receive msg {}", buffer_.toStdString());
        QDataStream stream(&buffer_, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_8);

        forever {
            //先解析头部
            if(!is_rcv_pending_){
                // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
                if (buffer_.size() < static_cast<int>(sizeof(quint16) * 2)) {
                    return; // 数据不够，等待更多数据
                }

                // 预读取消息ID和消息长度，但不从缓冲区中移除
                stream >> message_id_ >> message_len_;

                //将buffer 中的前四个字节移除
                buffer_ = buffer_.mid(sizeof(quint16) * 2);

                // 输出读取的数据
                SPDLOG_INFO("Message id {}, message len {}", message_id_, message_len_);
            }

            //buffer剩余长读是否满足消息体长度，不满足则退出继续等待接受
            if(buffer_.size() < message_len_){
                is_rcv_pending_ = true;
                return;
            }

            is_rcv_pending_ = false;
            // 读取消息体
            QByteArray messageBody = buffer_.mid(0, message_len_);
            SPDLOG_INFO("receive body msg is {}", messageBody.toStdString());

            buffer_ = buffer_.mid(message_len_);
            handleMsg(RequestId(message_id_),message_len_, messageBody);
        }

    });

    //5.15 之后版本
    QObject::connect(&ssl_socket_, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
        SPDLOG_ERROR("Socket error: {}", ssl_socket_.errorString().toStdString());
        switch (socketError) {
        case QTcpSocket::ConnectionRefusedError:
            SPDLOG_ERROR("Connection Refused!");
            emit sig_con_success(false);
            break;
        case QTcpSocket::RemoteHostClosedError:
            SPDLOG_ERROR("Remote Host Closed Connection!");
            break;
        case QTcpSocket::HostNotFoundError:
            SPDLOG_ERROR("Host Not Found!");
            emit sig_con_success(false);
            break;
        case QTcpSocket::SocketTimeoutError:
            SPDLOG_ERROR("Connection Timeout!");
            emit sig_con_success(false);
            break;
        case QTcpSocket::NetworkError:
            SPDLOG_ERROR("Network Error!");
            break;
        default:
            SPDLOG_ERROR("Other Error!");
            break;
        }
    });

    // 处理连接断开
    QObject::connect(&ssl_socket_, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
    });
    //连接发送信号用来发送数据
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    //注册消息
    initHandlers();
}

TcpMgr::~TcpMgr(){

}
void TcpMgr::initHandlers()
{
    //auto self = shared_from_this();
    handlers_.insert(RequestId::ID_CHAT_LOGIN_RSP, [this](RequestId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if(jsonDoc.isNull()){
            SPDLOG_WARN("Failed to create create QJsonDocument.");
            emit sig_login_failed(StatusCode::kErrJson);
            return;
        }
        
        if (!jsonDoc.isObject()) {
            SPDLOG_WARN("Failed to parese json");
            emit sig_login_failed(StatusCode::kErrJson);
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        auto status = static_cast<StatusCode>(jsonObj["status"].toInt());
        if(status != StatusCode::kSuccess){
            SPDLOG_WARN("Login failed, err is {}", static_cast<int>(status));
            emit sig_login_failed(status);
            return;
        }

        // auto uid = jsonObj["uid"].toInt();
        // auto name = jsonObj["name"].toString();
        // auto nick = jsonObj["nick"].toString();
        // auto icon = jsonObj["icon"].toString();
        // auto sex = jsonObj["sex"].toInt();
        // auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

        // UserMgr::Instance()->SetUserInfo(user_info);
        // UserMgr::Instance()->SetToken(jsonObj["token"].toString());
        // if(jsonObj.contains("apply_list")){
        //     UserMgr::Instance()->AppendApplyList(jsonObj["apply_list"].toArray());
        // }

        // //添加好友列表
        // if (jsonObj.contains("friend_list")) {
        //     UserMgr::Instance()->AppendFriendList(jsonObj["friend_list"].toArray());
        // }

        emit sig_swich_chatdlg();
    });


    handlers_.insert(RequestId::ID_SEARCH_USER_RSP, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Login Failed, err is Json Parse Err" << err;

            emit sig_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Login Failed, err is " << err;
            emit sig_user_search(nullptr);
            return;
        }
        auto search_info =  std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), jsonObj["name"].toString(),
                                                        jsonObj["nick"].toString(), jsonObj["desc"].toString(),
                                                        jsonObj["sex"].toInt(), jsonObj["icon"].toString());

        emit sig_user_search(search_info);
    });

    handlers_.insert(RequestId::ID_NOTIFY_ADD_FRIEND_REQ, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Login Failed, err is Json Parse Err" << err;

            emit sig_user_search(nullptr);
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Login Failed, err is " << err;
            emit sig_user_search(nullptr);
            return;
        }

        int from_uid = jsonObj["applyuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString desc = jsonObj["desc"].toString();
        QString icon = jsonObj["icon"].toString();
        QString nick = jsonObj["nick"].toString();
        int sex = jsonObj["sex"].toInt();

        auto apply_info = std::make_shared<AddFriendApply>(
            from_uid, name, desc,
            icon, nick, sex);

        emit sig_friend_apply(apply_info);
    });

    handlers_.insert(RequestId::ID_NOTIFY_AUTH_FRIEND_REQ, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int from_uid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();

        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);

        emit sig_add_auth_friend(auth_info);
    });

    handlers_.insert(RequestId::ID_ADD_FRIEND_RSP, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Add Friend Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Add Friend Failed, err is " << err;
            return;
        }

        qDebug() << "Add Friend Success " ;
    });


    handlers_.insert(RequestId::ID_AUTH_FRIEND_RSP, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);

        qDebug() << "Auth Friend Success " ;
    });


    handlers_.insert(RequestId::ID_TEXT_CHAT_MSG_RSP, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });

    handlers_.insert(RequestId::ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](RequestId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << static_cast<int>(id) << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = static_cast<int>(StatusCode::kErrJson);
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != static_cast<int>(StatusCode::kSuccess)) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });
}

void TcpMgr::handleMsg(RequestId id, int len, QByteArray data)
{
    auto find_iter =  handlers_.find(id);
    if(find_iter == handlers_.end()){
        qDebug()<< "not found id ["<< static_cast<int>(id) << "] to handle";
        return ;
    }

    find_iter.value()(id,len,data);
}

void TcpMgr::slot_connect_chat(const ServerInfo& server_info)
{
    SPDLOG_INFO("Connecting to server {}:{}", server_info.host.toStdString(), server_info.port.toStdString());
    host_ = server_info.host;
    port_ = static_cast<uint16_t>(server_info.port.toUInt());
    token_ = server_info.token;
    user_id_ = server_info.uid;
    ssl_socket_.connectToHostEncrypted(host_, port_);
}

void TcpMgr::slot_send_data(RequestId reqId, QByteArray dataBytes)
{
    uint16_t id = static_cast<uint16_t>(reqId);

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.length());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(dataBytes);

    // 发送数据
    ssl_socket_.write(block);
    qDebug() << "tcp mgr send byte data is " << block ;
}

WebSocketClient::WebSocketClient() : m_webSocket(new QWebSocket)
{
    // Configure SSL settings if necessary
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    m_webSocket->setSslConfiguration(sslConfig);

    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    // connect(m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
    //         this, &WebSocketClient::onSslErrors);
    connect(m_webSocket, &QWebSocket::sslErrors, this, &WebSocketClient::onSslErrors);
    QObject::connect(m_webSocket, &QWebSocket::stateChanged,
                     [](QAbstractSocket::SocketState state) {
                         switch (state) {
#if 0
                         case QAbstractSocket::ConnectedState:
                             SPDLOG_INFO("WebSocket connected.");
                             break;
                         case QAbstractSocket::UnconnectedState:
                             SPDLOG_INFO("WebSocket disconnected.");
                             break;
#endif
                         default:
                             break;
                         }
                     });
    //连接发送信号用来发送数据
    QObject::connect(this, &WebSocketClient::sig_send_data, this, &WebSocketClient::SlotSendData);
    QObject::connect(this, &WebSocketClient::SigSendText, this, &WebSocketClient::SlotSendText);

    InitHandlers();

}

void WebSocketClient::InitHandlers()
{
    handlers_[static_cast<int>(MessageId::MSG_CHAT_LOGIN)] = [this](const QJsonObject& json_obj) {
        auto status = static_cast<StatusCode>(json_obj["status"].toInt());
        if(status != StatusCode::kSuccess){
            SPDLOG_WARN("Login failed, err is {}", static_cast<int>(status));
            emit sig_login_failed(status);
            return;
        }

        //添加好友列表
        if (json_obj.contains("friends")) {
            UserMgr::Instance()->AppendFriendList(json_obj["friends"].toArray());
        }


        emit sig_swich_chatdlg();
    };

    handlers_[static_cast<int>(MessageId::ID_TEXT_CHAT_MSG_RSP)] = [this](const QJsonObject& json_obj) {
        auto status = static_cast<StatusCode>(json_obj["status"].toInt());
        if(status != StatusCode::kSuccess){
            SPDLOG_WARN("Login failed, err is {}", static_cast<int>(status));
            emit sig_login_failed(status);
            return;
        }

        // 接收信息
        if (!json_obj.contains("contents") || !json_obj["contents"].isArray() || !json_obj.contains("relateId"))
            return;

        auto relate_id = json_obj["relateId"].toInt();
        auto friend_ptr = UserMgr::Instance()->GetFriendByRelateId(relate_id);
        if (friend_ptr == nullptr)
            return;

        const auto& contents = json_obj["contents"].toArray();
        for (const auto &content : contents) {
            if (content.type() != QJsonValue::Object)
                continue;
            auto content_obj = content.toObject();
            friend_ptr->_chat_msgs.emplace_back(std::make_shared<TextChatData>(QString(""), content_obj["content"].toString(), 0, 0, content_obj["self"].toBool()));
            emit SigAppendMessage(friend_ptr, friend_ptr->_chat_msgs.back());
        }
    };
}

void WebSocketClient::SlotConnectChat(const ServerInfo &server_info)
{
    host_ = server_info.host;
    port_ = static_cast<uint16_t>(server_info.port.toUInt());
    token_ = server_info.token;
    // user_id_ = server_info.uid;

    QString url_str = "wss://" + host_ + ":" + QString::number(port_);
    SPDLOG_INFO("Connecting to server {}", url_str.toStdString());
    QUrl url(url_str);
    QNetworkRequest request(url);
    // Add the token to the request headers
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token_).toUtf8());

    // Open the WebSocket connection with the custom request
    m_webSocket->open(request);
}

void WebSocketClient::SlotSendData(MessageId msgId, QByteArray data)
{
    uint16_t id = static_cast<uint16_t>(msgId);

    QJsonObject json_obj;
    json_obj.insert("msgId", id);
    // json_obj.insert("text", data);


    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(data.length());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(data);

    // 发送数据
    m_webSocket->sendBinaryMessage(block);
    qDebug() << "tcp mgr send byte data is " << block ;
}

void WebSocketClient::SlotSendBinary(const QByteArray &data)
{

}

void WebSocketClient::SlotSendText(const QString &text)
{
    m_webSocket->sendTextMessage(text);
}

void WebSocketClient::onConnected()
{
    SPDLOG_INFO("Connected to server!");
    // Create and populate the QJsonObject
    QJsonObject jsonObj;
    jsonObj.insert("msgId", static_cast<int>(MessageId::MSG_CHAT_LOGIN));

#if 0
    // Convert QJsonObject to CBOR format
    QCborMap cborMap = QCborMap::fromJsonObject(jsonObj);
    QByteArray cborData = QCborValue(cborMap).toCbor();
    SPDLOG_INFO("Send data: {}", cborData.toStdString());
    m_webSocket->sendBinaryMessage(cborData);
#else
    // Convert QJsonObject to a compact JSON string
    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
    SPDLOG_INFO("Send data: {}", jsonData.toStdString());
    m_webSocket->sendTextMessage(QString::fromUtf8(jsonData));
#endif
    emit sig_swich_chatdlg();
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    SPDLOG_INFO("Message received from server: {}", message.toStdString());

    // 将QByteArray转换为QJsonDocument
    QJsonParseError parse_err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &parse_err);
    // 检查转换是否成功
    if(parse_err.error != QJsonParseError::NoError){
        SPDLOG_WARN("Failed to create QJsonDocument: ", parse_err.errorString().toStdString());
        emit sig_login_failed(StatusCode::kErrJson);
        return;
    }

    if (!jsonDoc.isObject()) {
        SPDLOG_WARN("Json is not an object.");
        emit sig_login_failed(StatusCode::kErrJson);
        return;
    }

    QJsonObject json_obj = jsonDoc.object();
    if (!json_obj.contains("msgId")) {
        SPDLOG_WARN("Invalid response, without msgId");
        return;
    }
    auto msg_id = json_obj.value("msgId").toInt();
    if (msg_id < 0 || msg_id > static_cast<int>(MessageId::kMessageIdNum)) {
        SPDLOG_WARN("Invalid response, illegal msgId");
        return;
    }
    handlers_[msg_id](json_obj);
}

void WebSocketClient::onDisconnected()
{
    SPDLOG_INFO("Websocket disconnected.");
}

void WebSocketClient::onSslErrors(const QList<QSslError> &errors)
{

    SPDLOG_ERROR("SSL errors occurred:");
    for (const QSslError &error : errors)
    {
        SPDLOG_ERROR(error.errorString().toStdString());
    }
    // Handle SSL errors appropriately
    // For testing purposes, you might choose to ignore SSL errors:
    // m_webSocket->ignoreSslErrors();
}
