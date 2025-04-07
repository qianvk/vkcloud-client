#include "chatitembase.h"

#include <QFont>
#include <QVBoxLayout>
#include <QLabel>

ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent)
    : QWidget(parent)
    , role_(role)
{
    p_name_lb_    = new QLabel();
    p_name_lb_->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    p_name_lb_->setFont(font);
    p_name_lb_->setFixedHeight(20);

    p_icon_lb_    = new QLabel();
    p_icon_lb_->setScaledContents(true);
    p_icon_lb_->setFixedSize(42, 42);

    p_bubble_       = new QWidget();

    QGridLayout *pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setContentsMargins(3, 3, 3, 3);
    QSpacerItem*pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    if(role_ == ChatRole::Self)
    {
        p_name_lb_->setContentsMargins(0,0,8,0);
        p_name_lb_->setAlignment(Qt::AlignRight);
        pGLayout->addWidget(p_name_lb_, 0,1, 1,1);
        pGLayout->addWidget(p_icon_lb_, 0, 2, 2,1, Qt::AlignTop);
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);
        pGLayout->addWidget(p_bubble_, 1,1, 1,1);
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
    }else{
        p_name_lb_->setContentsMargins(8,0,0,0);
        p_name_lb_->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(p_icon_lb_, 0, 0, 2,1, Qt::AlignTop);
        pGLayout->addWidget(p_name_lb_, 0,1, 1,1);
        pGLayout->addWidget(p_bubble_, 1,1, 1,1);
        pGLayout->addItem(pSpacer, 2, 2, 1, 1);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
    }
    this->setLayout(pGLayout);
}

void ChatItemBase::SetUserName(const QString &name)
{
    p_name_lb_->setText(name);
}

void ChatItemBase::SetUserIcon(const QPixmap &icon)
{
    p_icon_lb_->setPixmap(icon);
}

void ChatItemBase::SetWidget(QWidget *w)
{
    QGridLayout *pGLayout = (qobject_cast<QGridLayout *>)(this->layout());
    pGLayout->replaceWidget(p_bubble_, w);
    delete p_bubble_;
    p_bubble_ = w;
}
