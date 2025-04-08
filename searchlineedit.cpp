#include "searchlineedit.h"
#include "usermanager.h"
#include "chatuseritem.h"

#include <QListWidgetItem>
#include <spdlog/spdlog.h>

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , max_len_(0)
{
    connect(this, &SearchLineEdit::textChanged, this, &SearchLineEdit::LimitTextLength);
    connect(this, &SearchLineEdit::textChanged, this, &SearchLineEdit::SlotInstantSearch);
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

void SearchLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    QPoint relative_pos = this->pos();
    // relative_pos = this->mapTo(this->parentWidget()->parentWidget(), this->pos());
    relative_pos.setY(relative_pos.y() + this->height());
    emit SigShowSearchList(relative_pos, this->parentWidget()->width() - 18, this->parentWidget()->parentWidget());
}

void SearchLineEdit::LimitTextLength(const QString &text)
{
    if (text.size() <= 0)
        return;

    QByteArray byte_arr = text.toUtf8();
    if (byte_arr.size() > max_len_) {
        byte_arr = byte_arr.left(max_len_);
        this->setText(QString::fromUtf8(byte_arr));
    }
}

void SearchLineEdit::SlotInstantSearch(const QString &text)
{
    InstantSearchRets search_rets;
    if (!text.isEmpty()) {
        auto friends = UserMgr::Instance()->GetFriendList();
        for(auto &friend_ele : friends){
            if (!friend_ele->_nick.contains(text))
                continue;

            SPDLOG_INFO("User {} contains {}", friend_ele->_nick.toStdString(), text.toStdString());
            auto *chat_item = new ChatUserItem();
            chat_item->SetInfo(friend_ele);
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(chat_item->sizeHint());
            search_rets.emplace_back(item, chat_item);
        }
    }

    emit SigInstantSearchRets(search_rets);
}
