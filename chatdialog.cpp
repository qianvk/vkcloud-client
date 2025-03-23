#include "chatdialog.h"
#include "ui_chatdialog.h"

#include <QAction>

#include "chatuseritem.h"
#include "loadingdialog.h"

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

    connect(ui->chat_user_list, &ChatUserList::SigLoadingChatUser, this, &ChatDialog::SlotLoadingChatUser);
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
    for (int i = 0; i < 100; ++i) {
        auto *chat_item = new ChatUserItem();
        chat_item->SetInfo("qianvk", "", "hello!");
        auto *item = new QListWidgetItem;
        item->setSizeHint(chat_item->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_item);
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
