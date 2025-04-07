#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmanager.h"
#include "configurationmanager.h"
#include "tcpmanager.h"
#include "usermanager.h"


#include <QJsonObject>
#include <QJsonDocument>
#include <spdlog/spdlog.h>


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->password_edit->setEchoMode(QLineEdit::Password);

    connect(ui->register_button, &QPushButton::clicked, this, &LoginDialog::SwitchRegister);
    connect(HttpManager::Instance().get(), &HttpManager::SigLoginFinish, this, &LoginDialog::SlotLoginFinish);

    // connect(this, &LoginDialog::sig_connect_chat, TcpMgr::Instance().get(), &TcpMgr::slot_connect_chat);
    // connect(TcpMgr::Instance().get(), &TcpMgr::sig_con_success, this, &LoginDialog::SlotConnectChatResult);
    // connect(TcpMgr::Instance().get(), &TcpMgr::sig_login_failed, this, &LoginDialog::SlotLoginFailed);


    connect(this, &LoginDialog::sig_connect_chat, WebSocketClient::Instance().get(), &WebSocketClient::SlotConnectChat);
    connect(WebSocketClient::Instance().get(), &WebSocketClient::sig_con_success, this, &LoginDialog::SlotConnectChatResult);
    connect(WebSocketClient::Instance().get(), &WebSocketClient::sig_login_failed, this, &LoginDialog::SlotLoginFailed);
    InitHandlers();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_button_clicked()
{
    if (!CheckUserValid() || ! CheckPasswordValid())
        return;

    auto email = ui->email_edit->text();
    auto password = ui->password_edit->text();

    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["password"] = password;
    HttpManager::Instance()->PostHttpRequest(QUrl(APPCONFIG.GetVKCloudUrlPrefix() + "/api/login"), json_obj, RequestId::kLogin, Module::kLogin);
    SPDLOG_INFO("start logining");
}

void LoginDialog::SlotLoginFinish(RequestId req_id, const QJsonObject &json_obj)
{
    SPDLOG_INFO(static_cast<int>(json_obj["status"].toInt()));
    auto status = static_cast<StatusCode>(json_obj["status"].toInt());
    if (status == StatusCode::kErrJson || status == StatusCode::kErrNetwork) {
        SPDLOG_ERROR(EnumToString(status));
        ShowTip(QString::fromUtf8(EnumToString(status)), false);
        return;
    }

    if (status != StatusCode::kSuccess) {
        QString error = json_obj["error"].toString();
        SPDLOG_ERROR(error.toStdString());
        ShowTip(error, false);
        return;
    }

    QJsonDocument doc(json_obj);
    SPDLOG_INFO("Login succeed, recieve message{}", doc.toJson(QJsonDocument::Compact).toStdString());

    auto find_iter = handlers_.find(req_id);
    if (find_iter == handlers_.end()) {
        SPDLOG_ERROR("Handler lack");
        ShowTip(tr("客户端内部错误"), false);
        return;
    }

    find_iter->second(json_obj);
}

void LoginDialog::SlotConnectChatResult(bool is_success)
{
    if(is_success){
        ShowTip(tr("聊天服务连接成功，正在登录..."),true);
    }else{
        ShowTip(tr("网络异常，请稍后再试"),false);
        EnableLoginLabel(true);
    }
}

void LoginDialog::SlotLoginFailed(StatusCode status)
{
    ShowTip(tr("登录失败，请稍后再试"), false);
    EnableLoginLabel(true);
}

void LoginDialog::InitHandlers()
{
    //注册获取登录回包逻辑
    handlers_.emplace(RequestId::kLogin, [this](const QJsonObject &json_obj){
        auto status = static_cast<StatusCode>(json_obj["status"].toInt());
        if(status != StatusCode::kSuccess){
            ShowTip(tr("参数错误"),false);
            EnableLoginLabel(true);
            return;
        }

        //发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.host = json_obj["host"].toString();
        si.port = QString::number(json_obj["port"].toInt());
        si.token = json_obj["token"].toString();

        QString nick = json_obj["nick"].toString();
        QString avatar = json_obj["avatar"].toString();
        auto user_info = std::make_shared<UserInfo>(nick, avatar);
        UserMgr::Instance()->SetUserInfo(user_info);
        emit sig_connect_chat(si);
    });

}

void LoginDialog::EnableLoginLabel(bool enabled)
{
    ui->login_button->setEnabled(enabled);
    // ui->register_button->setEnabled(enabled);
}

bool LoginDialog::CheckUserValid()
{
    auto user = ui->email_edit->text();
    if (!user.isEmpty())
        return true;

    ShowTip("用户名不能为空", false);
    return false;
}

bool LoginDialog::CheckPasswordValid()
{
    auto password = ui->password_edit->text();
    if (password.size() < 5 || password.size() > 16)
    {
        ShowTip("密码应至少5位，且不多于16位", false);
        return false;
    }

    return true;
}

void LoginDialog::ShowTip(const QString &tip, bool is_ok)
{
    ui->tip_label->setText(tip);
    ui->tip_label->setProperty("state", is_ok ? "normal" : "error");
    Repolish(ui->tip_label);
}

