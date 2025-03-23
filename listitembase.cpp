#include "listitembase.h"

#include <QStyleOption>
#include <QPainter>

ListItemBase::ListItemBase(QWidget *parent)
    : QWidget{parent}
{}

void ListItemBase::SetItemType(ListItemType item_type)
{
    item_type_ = item_type;
}

ListItemType ListItemBase::GetItemType()
{
    return item_type_;
}

void ListItemBase::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
