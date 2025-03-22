#include "global.h"

#include <QWidget>
#include <QStyle>

std::function<void(QWidget*)> Repolish = [](QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};
