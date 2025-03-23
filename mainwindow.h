#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class LoginDialog;
class RegisterDialog;
class ChatDialog;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchRegister();
    void SlotSwitchLogin();
    void SlotSwitchChat();

private:
    Ui::MainWindow *ui;
    LoginDialog* login_dlg_;
    RegisterDialog* register_dlg_;
    ChatDialog *chat_dlg_;
};
#endif // MAINWINDOW_H
