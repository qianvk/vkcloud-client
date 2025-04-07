#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"

#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "spdlog/spdlog.h"

#include "configurationmanager.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    InitHandlers();

    ui->setupUi(this);
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->tip_label->setProperty("state", "normal");
    Repolish(ui->tip_label);
    ui->stackedWidget->setCurrentWidget(ui->page);

    connect(HttpManager::Instance().get(), &HttpManager::sig_register_finish, this, &RegisterDialog::slot_register_finish);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_button_clicked()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        QJsonObject json_obj;
        json_obj["email"] = email;
        //发送http请求获取验证码
        HttpManager::Instance()->PostHttpRequest(QUrl(APPCONFIG.GetVKCloudUrlPrefix() + "/api/verify-code"), json_obj, RequestId::kGetVerifyCode, Module::kRegister);
    }else{
        //提示邮箱不正确
        ShowTip(tr("邮箱地址不正确"), false);
    }
}

void RegisterDialog::slot_register_finish(RequestId req_id, const QJsonObject &json_obj)
{
    auto status = static_cast<StatusCode>(json_obj["status"].toInt());
    if (status == StatusCode::kErrJson || status == StatusCode::kErrNetwork) {
        spdlog::error(EnumToString(status));
        std::string_view sv = EnumToString(status);
        ShowTip(QString::fromUtf8(sv.data(), sv.size()), false);
        return;
    }

    auto iter = handlers_.find(req_id);
    if (iter == handlers_.end())
        return;

    iter.value()(json_obj);
}

void RegisterDialog::ShowTip(const QString &tip, bool is_ok)
{
    ui->tip_label->setText(tip);
    ui->tip_label->setProperty("state", is_ok ? "normal" : "error");
    Repolish(ui->tip_label);
}

void RegisterDialog::InitHandlers()
{
    handlers_.insert(RequestId::kGetVerifyCode, [this](const QJsonObject& json_obj) {
        auto status = json_obj["status"].toInt();
        if (status != static_cast<int>(StatusCode::kSuccess)) {
            ShowTip(json_obj["error"].toString(), false);
            return;
        }

        ShowTip(tr("验证码已发送到邮箱，注意查收"), true);
    });

    handlers_.insert(RequestId::kRegisterUser, [this](const QJsonObject& json_obj) {
        auto status = json_obj["status"].toInt();
        if (status != static_cast<int>(StatusCode::kSuccess)) {
            ShowTip(tr("注册失败请重试"), false);
            // ShowTip(json_obj["error"].toString(), false);
            return;
        }

        ShowTip(tr("注册成功"), true);
        SwitchTipPage();
    });
}

void RegisterDialog::SwitchTipPage()
{
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    countdown_timer_ = new QTimer(this);
    countdown_timer_->start(1000);
    countdown_ = 5;
    connect(countdown_timer_, &QTimer::timeout, [this]() {
        if (countdown_ == 0) {
            countdown_timer_->stop();
            emit SigSwitchLogin();
            return;
        }

        --countdown_;
        ui->switch_tip_label->setText(QString(tr("注册成功，%1 s后返回登录").arg(countdown_)));
    });

}


void RegisterDialog::on_confirm_button_clicked()
{
    // if(ui->user_edit->text() == ""){
    //     ShowTip(tr("用户名不能为空"), false);
    //     return;
    // }

    // if(ui->email_edit->text() == ""){
    //     ShowTip(tr("邮箱不能为空"), false);
    //     return;
    // }

    // if(ui->password_edit->text() == ""){
    //     ShowTip(tr("密码不能为空"), false);
    //     return;
    // }

    // if(ui->confirm_edit->text() == ""){
    //     ShowTip(tr("确认密码不能为空"), false);
    //     return;
    // }

    // if(ui->confirm_edit->text() != ui->password_edit->text()){
    //     ShowTip(tr("密码和确认密码不匹配"), false);
    //     return;
    // }

    // if(ui->code_edit->text() == ""){
    //     ShowTip(tr("验证码不能为空"), false);
    //     return;
    // }

    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    json_obj["password"] = ui->password_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["varifycode"] = ui->code_edit->text();
    // SwitchTipPage();
    HttpManager::Instance()->PostHttpRequest(QUrl(APPCONFIG.GetVKCloudUrlPrefix() + "/api/register"),
                                        json_obj, RequestId::kRegisterUser,Module::kRegister);
}


void RegisterDialog::on_back_login_button_clicked()
{
    countdown_timer_->stop();
    emit SigSwitchLogin();
}


void RegisterDialog::on_cancel_button_clicked()
{
    emit SigSwitchLogin();
}

