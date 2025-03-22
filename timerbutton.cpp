#include "timerbutton.h"

#include <QTimer>
#include <QMouseEvent>

TimerButton::TimerButton(QWidget *parent)
    : QPushButton(parent)
    , timer_(new QTimer(this))
    , counter_{10}
{
    connect(timer_, &QTimer::timeout, [this]() {
        --counter_;
        if (counter_ <= 0) {
            this->setText(tr("获取"));
            this->setEnabled(true);
            timer_->stop();
            counter_ = 10;
            return;
        }

        this->setText(QString::number(counter_));
    });
}

TimerButton::~TimerButton()
{
    timer_->stop();
}

void TimerButton::mouseReleaseEvent(QMouseEvent *mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton) {
        this->setText(QString::number(counter_));
        this->setEnabled(false);
        timer_->start(1000);
        emit clicked();
    }

    QPushButton::mouseMoveEvent(mouse_event);
}
