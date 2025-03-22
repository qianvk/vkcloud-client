#ifndef TIMERBUTTON_H
#define TIMERBUTTON_H

#include <QPushButton>

class TimerButton : public QPushButton
{
public:
    TimerButton(QWidget* parent = nullptr);
    ~TimerButton();

    virtual void mouseReleaseEvent(QMouseEvent* mouse_event) override;

private:
    QTimer *timer_;
    int counter_;
};

#endif // TIMERBUTTON_H
