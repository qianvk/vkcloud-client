#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void SigSwitchLogin();

private slots:
    void on_get_code_button_clicked();
    void slot_register_finish(RequestId req_id, const QJsonObject &json_obj);

    void on_confirm_button_clicked();

    void on_back_login_button_clicked();

    void on_cancel_button_clicked();

private:
    void ShowTip(const QString& tip, bool is_ok);
    void InitHandlers();
    void SwitchTipPage();

    Ui::RegisterDialog *ui;
    QMap<RequestId, std::function<void(const QJsonObject&)>> handlers_;
    QTimer* countdown_timer_;
    int countdown_;
};

#endif // REGISTERDIALOG_H
