#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class LoginDialog;
class RegisterDialog;
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

private:
    Ui::MainWindow *ui;
    LoginDialog* login_dlg_;
    RegisterDialog* register_dlg_;
};
#endif // MAINWINDOW_H
