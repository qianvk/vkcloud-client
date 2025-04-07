#include "chatview.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>

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

    auto *w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);

    auto *p_vertical_layout = new QVBoxLayout();
    p_vertical_layout->addWidget(new QWidget(), 100'000);
    w->setLayout(p_vertical_layout);
    p_chat_area_->setWidget(w);

    p_chat_area_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *p_vertical_scroll_bar = p_chat_area_->verticalScrollBar();
    connect(p_vertical_scroll_bar, &QScrollBar::rangeChanged, this, &ChatView::SlotVScrollBarMoved);

    auto *p_horizontal_layout = new QHBoxLayout();
    p_horizontal_layout->addWidget(p_vertical_scroll_bar, 0, Qt::AlignRight);
    p_horizontal_layout->setContentsMargins(0, 0, 0, 0);
    p_chat_area_->setLayout(p_horizontal_layout);
    p_vertical_scroll_bar->setHidden(true);

    p_chat_area_->setWidgetResizable(true);
    p_chat_area_->installEventFilter(this);
}

void ChatView::AppendChatItem(QWidget *item)
{
    auto *vl = qobject_cast<QVBoxLayout*>(p_chat_area_->widget()->layout());
    vl->insertWidget(vl->count() - 1, item);
    is_loading_bottom_ = true;
}

void ChatView::ClearChatItem()
{
    auto *vl = qobject_cast<QVBoxLayout*>(p_chat_area_->widget()->layout());
    QLayoutItem *item = nullptr;
    while (vl->count() > 1 && (item = vl->takeAt(0))) {
        if (QWidget* widget = item->widget()) {
            delete widget; // Delete the widget to free memory
        }
        delete item; // Delete the layout item itself
    }

}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter && watched == p_chat_area_)
        p_chat_area_->verticalScrollBar()->setHidden(p_chat_area_->verticalScrollBar()->maximum() == 0);
    else if (event->type() == QEvent::Leave && watched == p_chat_area_)
        p_chat_area_->verticalScrollBar()->setHidden(true);

    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::SlotVScrollBarMoved(int min, int max)
{
    if (is_loading_bottom_) {
        QScrollBar *p_vertical_scroll_bar = p_chat_area_->verticalScrollBar();
        p_vertical_scroll_bar->setSliderPosition(p_vertical_scroll_bar->maximum());
        QTimer::singleShot(500, [this]()
                           {
            is_loading_bottom_ = false;
        });
    }
}
