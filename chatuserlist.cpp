#include "chatuserlist.h"

#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>

#include "global.h"

ChatUserList::ChatUserList(QWidget *parent)
    : QListWidget(parent)
    , scroll_timer_(this)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);

    connect(&scroll_timer_, &QTimer::timeout, this, &ChatUserList::SlotPerformSmoothScroll);
    scroll_timer_.setInterval(16); // Approximately 60 FPS
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter)
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        else if (event->type() == QEvent::Leave)
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        else if (event->type() == QEvent::Wheel) {
            auto *wheel_event = static_cast<QWheelEvent*>(event);
            // int num_degree = wheel_event->angleDelta().y();
            // int num_steps = num_degree;

            // QScrollBar *scroll_bar = this->verticalScrollBar();
            // scroll_bar->setValue(scroll_bar->value() - num_steps);
            // int max_scroll_value = scroll_bar->maximum();
            // int current_scroll_value = scroll_bar->value();
            // if (max_scroll_value <= current_scroll_value) {
            //     emit SigLoadingChatUser();
            // }

            // Determine the scrolling delta
            QPoint delta = wheel_event->pixelDelta();
            if (delta.isNull()) {
                // Fallback for devices that don't support pixelDelta
                delta = wheel_event->angleDelta() / 8;
            }

            // Accumulate the scrolling delta
            accumulated_delta_ += delta;

            // Start the smooth scrolling timer if not already running
            if (!scroll_timer_.isActive()) {
                scroll_timer_.start();
            }

            // Accept the event to prevent further processing
            event->accept();
        }
    }

    return QListWidget::eventFilter(watched, event);
}

void ChatUserList::SlotPerformSmoothScroll()
{
    // Determine the amount to scroll
    int step = 1; // Adjust this value for smoother or faster scrolling
    int verticalStep = qMin(step, qAbs(accumulated_delta_.y()));
    int horizontalStep = qMin(step, qAbs(accumulated_delta_.x()));

    // Scroll vertically
    if (accumulated_delta_.y() != 0) {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (verticalStep * sign(accumulated_delta_.y())));
        accumulated_delta_.ry() -= verticalStep * sign(accumulated_delta_.y());
    }

    QScrollBar *scroll_bar = this->verticalScrollBar();
    int max_scroll_value = scroll_bar->maximum();
    int current_scroll_value = scroll_bar->value();
    if (max_scroll_value <= current_scroll_value) {
        emit SigLoadingChatUser();
    }

    // Scroll horizontally
    if (accumulated_delta_.x() != 0) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (horizontalStep * sign(accumulated_delta_.x())));
        accumulated_delta_.rx() -= horizontalStep * sign(accumulated_delta_.x());
    }

    // Stop the timer if there's no more scrolling to do
    if (accumulated_delta_.isNull()) {
        scroll_timer_.stop();
    }
}
