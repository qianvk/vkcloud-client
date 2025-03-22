#include "logindialog.h"
#include "ui_logindialog.h"

#include <QJsonObject>
#include <httpmanager.h>

#include "spdlog/spdlog.h"

#include "configurationmanager.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->password_edit->setEchoMode(QLineEdit::Password);

    connect(ui->register_button, &QPushButton::clicked, this, &LoginDialog::SwitchRegister);
    connect(HttpManager::Instance().get(), &HttpManager::SigLoginFinish, this, &LoginDialog::SlotLoginFinish);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_button_clicked()
{
    if (!CheckUserValid() || ! CheckPasswordValid())
        return;

    auto user = ui->user_edit->text();
    auto password = ui->password_edit->text();

    QJsonObject json_obj;
    json_obj["username"] = user;
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

    SPDLOG_INFO("login succeed");
    ShowTip("登录成功", true);
}

bool LoginDialog::CheckUserValid()
{
    auto user = ui->user_edit->text();
    if (!user.isEmpty())
        return true;

    ShowTip("用户名不能为空", false);
    return false;
}

bool LoginDialog::CheckPasswordValid()
{
    auto password = ui->password_edit->text();
    if (password.size() < 6 || password.size() > 16)
    {
        ShowTip("密码应至少6位，且不多于16位", false);
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

