#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    void SwitchRegister();
    void sig_connect_chat(const ServerInfo& si);

private slots:
    void on_login_button_clicked();
    void SlotLoginFinish(RequestId req_id, const QJsonObject& json_obj);
    void SlotConnectChatResult(bool is_success);
    void SlotLoginFailed(StatusCode status);

private:
    void InitHandlers();
    void EnableLoginLabel(bool enabled);

    bool CheckUserValid();
    bool CheckPasswordValid();
    void ShowTip(const QString& tip, bool is_ok);

private:
    Ui::LoginDialog *ui;
    std::unordered_map<RequestId, std::function<void(const QJsonObject&)>> handlers_;
};

#endif // LOGINDIALOG_H
