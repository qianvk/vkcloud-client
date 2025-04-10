#include "searchlineedit.h"
#include "usermanager.h"
#include "chatuseritem.h"
#include "databasemanager.h"

#include <QListWidgetItem>
#include <spdlog/spdlog.h>
#include <QSqlQuery>
#include <QSqlError>

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
        // 1. Query all message contains text
        QSqlQuery query(DatabaseManager::Instance()->GetChatDb());
        query.prepare(R"(
            SELECT relate_id, content FROM message WHERE content LIKE :text
            ORDER BY relate_id ASC
        )");
        query.bindValue(":text", "%" + text + "%");
        if (!query.exec()) {
            SPDLOG_ERROR("Search query execution failed: {}", query.lastError().text().toStdString());
            emit SigInstantSearchRets(search_rets);
            return;
        }

        if (query.next()) {
            uint32_t last_relate_id = query.value("relate_id").toInt();
            QString content = query.value("content").toString();
            int count = 1;

            while (query.next()) {
                uint32_t relate_id = query.value("relate_id").toInt();
                if (relate_id == last_relate_id)
                    ++count;
                else {
                    auto *chat_item = new ChatUserItem();
                    if (count > 1)
                        chat_item->SetInfo(QString::number(last_relate_id), "", QString::number(count) + " related chats");
                    else
                        chat_item->SetInfo(QString::number(last_relate_id), "", content);
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setSizeHint(chat_item->sizeHint());
                    search_rets.emplace_back(item, chat_item);

                    count = 1;
                    content = query.value("content").toString();
                }
            }

            auto *chat_item = new ChatUserItem();
            if (count > 1)
                chat_item->SetInfo(QString::number(last_relate_id), "", QString::number(count) + "related chats");
            else
                chat_item->SetInfo(QString::number(last_relate_id), "", content);
            QListWidgetItem *item = new QListWidgetItem;
            item->setSizeHint(chat_item->sizeHint());
            search_rets.emplace_back(item, chat_item);
        }

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
