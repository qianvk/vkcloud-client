#include "searchlineedit.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , max_len_(0)
{
    connect(this, &SearchLineEdit::textChanged, this, &SearchLineEdit::LimitTextLength);
}

void SearchLineEdit::SetMaxLength(int max_len)
{
    max_len_ = max_len;
}

void SearchLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit SigFocusOut();
}

void SearchLineEdit::LimitTextLength(QString text)
{
    if (text.size() <= 0)
        return;

    QByteArray byte_arr = text.toUtf8();
    if (byte_arr.size() > max_len_) {
        byte_arr = byte_arr.left(max_len_);
        this->setText(QString::fromUtf8(byte_arr));
    }
}
