#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "logindialog.h"
#include "registerdialog.h"
#include "chatdialog.h"
#include "tcpmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , login_dlg_(new LoginDialog(this))
{
    ui->setupUi(this);

    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg_);

    connect(login_dlg_, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchRegister);
    connect(WebSocketClient::Instance().get(), &WebSocketClient::sig_swich_chatdlg, this, &MainWindow::SlotSwitchChat);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchRegister()
{
    register_dlg_ = new RegisterDialog(this);
    register_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(register_dlg_);

    connect(register_dlg_, &RegisterDialog::SigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
}

void MainWindow::SlotSwitchLogin()
{
    login_dlg_ = new LoginDialog(this);
    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg_);
    connect(login_dlg_, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchRegister);
}

void MainWindow::SlotSwitchChat()
{
    chat_dlg_ = new ChatDialog(this);
    chat_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(chat_dlg_);
    this->setMinimumSize(QSize(1050, 900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
