#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QWidget>

#include "global.h"

class QLabel;
class BubbleFrame;

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    void SetUserName(const QString& name);
    void SetUserIcon(const QPixmap& icon);
    void SetWidget(QWidget *w);

private:
    ChatRole role_;
    QLabel *p_name_lb_;
    QLabel *p_icon_lb_;
    QWidget *p_bubble_;
};

#endif // CHATITEMBASE_H
