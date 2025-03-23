#ifndef CHATUSERITEM_H
#define CHATUSERITEM_H

#include "listitembase.h"

namespace Ui {
class ChatUserItem;
}

class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

    QSize sizeHint() const override {
        return QSize(250, 70);
    }

    void SetInfo(QString name, QString head, QString msg);

private:
    Ui::ChatUserItem *ui;
    QString _name;
    QString _head;
    QString _msg;
};

#endif // CHATUSERITEM_H
