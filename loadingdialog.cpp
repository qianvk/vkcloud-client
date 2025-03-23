#include "loadingdialog.h"
#include "ui_loadingdialog.h"

#include <QMovie>

LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(parent->size());

    auto *movie = new QMovie(":res/loading.gif");
    ui->loading_lb->setMovie(movie);
    movie->start();
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}
