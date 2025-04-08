#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "usermanager.h"
#include "searchlist.h"
#include "chatuseritem.h"
#include "loadingdialog.h"

#include <spdlog/spdlog.h>
#include <QAction>


ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , _b_loading(false)
    , ui(new Ui::ChatDialog)
    , _mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode)
{
    ui->setupUi(this);
    ui->search_edit->SetMaxLength(15);

    auto *search_action = new QAction(ui->search_edit);
    search_action->setIcon(QIcon(":/resource/search.png"));
    ui->search_edit->addAction(search_action, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    auto *clear_action = new QAction(ui->search_edit);
    clear_action->setIcon(QIcon(":/resource/transparent.png"));
    ui->search_edit->addAction(clear_action, QLineEdit::TrailingPosition);

    connect(ui->search_edit, &SearchLineEdit::textChanged, [clear_action](const QString& text) {
        if (text.isEmpty())
            clear_action->setIcon(QIcon(":/resource/transparent.png"));
        else
            clear_action->setIcon(QIcon(":/resource/clear.png"));
    });

    connect(clear_action, &QAction::triggered, [this, clear_action]() {
        if (!ui->search_edit->text().isEmpty()) {
            ui->search_edit->clear();
            clear_action->setIcon(QIcon(":/resource/transparent.png"));
        }

        ui->search_edit->clearFocus();
    });

    ShowSearch(false);
    AddChatUserList();
    // ui->chat_user_list->setCurrentRow(0);
    if (ui->chat_user_list->count() > 0)
        SlotChangeChatInfo(ui->chat_user_list->item(0), nullptr);

    connect(ui->chat_user_list, &ChatUserList::SigLoadingChatUser, this, &ChatDialog::SlotLoadingChatUser);
    connect(ui->chat_user_list, &ChatUserList::currentItemChanged, this, &ChatDialog::SlotChangeChatInfo);

    connect(ui->search_edit, &SearchLineEdit::SigShowSearchList, this, &ChatDialog::SlotShowSearchList);
    connect(ui->search_edit, &SearchLineEdit::SigInstantSearchRets, this, &ChatDialog::SlotInstantSearchRets);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::ShowSearch(bool is_search)
{
    if(is_search){
        ui->chat_user_list->hide();
        // ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }else if(_state == ChatUIMode::ChatMode){
        ui->chat_user_list->show();
        // ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
        // ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }else if(_state == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->search_list->hide();
        // ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
        // ui->search_list->CloseFindDlg();
        ui->search_edit->clear();
        ui->search_edit->clearFocus();
    }

}

void ChatDialog::AddChatUserList()
{
    auto friend_list = UserMgr::Instance()->GetChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto &friend_ele : friend_list){
            // auto find_iter = _chat_items_added.find(friend_ele->_uid);
            // if(find_iter != _chat_items_added.end()){
            //     continue;
            // }
            auto *chat_item = new ChatUserItem();
            chat_item->SetInfo(friend_ele);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_item->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_item);
            // _chat_items_added.insert(friend_ele->_uid, item);
        }

        //更新已加载条目
        UserMgr::Instance()->UpdateChatLoadedCount();
    }
}

void ChatDialog::SlotLoadingChatUser()
{
    if (_b_loading)
        return;

    _b_loading = true;
    auto *loading_dlg = new LoadingDialog(this);
    loading_dlg->setModal(true);
    AddChatUserList();
    loading_dlg->deleteLater();
    _b_loading = false;
}

void ChatDialog::SlotChangeChatInfo(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        return;

    QWidget *widget = ui->chat_user_list->itemWidget(current);
    if (!widget)
        return;

    auto *chat_item = qobject_cast<ChatUserItem*>(widget);
    if (!chat_item)
        return;

    ui->chat_page->SetUserInfo(chat_item->GetUserInfo());
}

void resizeListWidgetToContents(QListWidget* list, int width = 0) {
    int height = list->count() * list->sizeHintForRow(0) + 2 * list->frameWidth();
    if (width == 0)
        width = list->sizeHintForColumn(0) + 2 * list->frameWidth();
    list->setFixedSize(width, height);
}


void ChatDialog::SlotShowSearchList(const QPoint &relative_pos, int width, QWidget *parent)
{
    if (!search_list_) {
        search_list_ = new SearchList(parent);
        search_list_->move(relative_pos);
        search_list_->raise();
        search_list_->show();
        SPDLOG_INFO("x {}, y {}, width {}.", relative_pos.x(), relative_pos.y(), width);
    }

    // auto friend_list = UserMgr::Instance()->GetFriendList();
    // for(auto &friend_ele : friend_list){
    //     SPDLOG_INFO("User {}.", friend_ele->_nick.toStdString());
    //     auto *chat_item = new ChatUserItem();
    //     chat_item->SetInfo(friend_ele);
    //     QListWidgetItem *item = new QListWidgetItem;
    //     item->setSizeHint(QSize(width, chat_item->sizeHint().height()));
    //     search_list_->addItem(item);
    //     search_list_->setItemWidget(item, chat_item);
    //     resizeListWidgetToContents(search_list_, width);
    // }
}

void ChatDialog::SlotInstantSearchRets(const InstantSearchRets &search_rets)
{
    search_list_->clear();
    for (const auto& search_pair : search_rets) {
        search_list_->addItem(search_pair.first);
        search_list_->setItemWidget(search_pair.first, search_pair.second);
    }

    resizeListWidgetToContents(search_list_, 232);
}
