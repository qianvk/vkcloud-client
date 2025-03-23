#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QWidget>
#include "global.h"

class ListItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent = nullptr);

    void SetItemType(ListItemType item_type);
    ListItemType GetItemType();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ListItemType item_type_;

signals:
};

#endif // LISTITEMBASE_H
