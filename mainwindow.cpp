#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "logindialog.h"
#include "registerdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , login_dlg_(new LoginDialog(this))
{
    ui->setupUi(this);

    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg_);

    connect(login_dlg_, &LoginDialog::SwitchRegister, this, &MainWindow::SlotSwitchRegister);
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
