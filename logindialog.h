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

private slots:
    void on_login_button_clicked();
    void SlotLoginFinish(RequestId req_id, const QJsonObject& json_obj);

private:
    bool CheckUserValid();
    bool CheckPasswordValid();
    void ShowTip(const QString& tip, bool is_ok);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
