#include "chatuseritem.h"
#include "ui_chatuseritem.h"

#include <QPixmap>

ChatUserItem::ChatUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}

void ChatUserItem::SetInfo(QString name, QString head, QString msg)
{
    _name = name;
    _head = head;
    _msg = msg;

    QPixmap pixmap(head);
    ui->avatar_lb->setPixmap(pixmap.scaled(ui->avatar_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->avatar_lb->setScaledContents(true);


    ui->username_lb->setText(name);
    ui->chat_history_lb->setText(msg);
}


