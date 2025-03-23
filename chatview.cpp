#include "chatview.h"

#include <QVBoxLayout>
#include <QScrollArea>

ChatView::ChatView(QWidget *parent)
    : QWidget{parent}
    , is_loading_bottom_(false)
    , p_chat_area_(new QScrollArea())
{
    QVBoxLayout *p_main_layout = new QVBoxLayout();
    this->setLayout(p_main_layout);
    p_main_layout->setContentsMargins(0, 0, 0, 0);

    p_chat_area_->setObjectName("chat_area");
    p_main_layout->addWidget(p_chat_area_);
}
