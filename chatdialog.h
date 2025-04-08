#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

#include "global.h"
#include "searchlineedit.h"

namespace Ui {
class ChatDialog;
}

class QListWidgetItem;
class SearchList;
class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    void ShowSearch(bool is_search);
    void AddChatUserList();

private slots:
    void SlotLoadingChatUser();
    void SlotChangeChatInfo(QListWidgetItem *current, QListWidgetItem *previous);
    void SlotShowSearchList(const QPoint &relative_pos, int width, QWidget *parent);
    void SlotInstantSearchRets(const InstantSearchRets& search_rets);

private:
    bool _b_loading;
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    SearchList *search_list_{nullptr};
};

#endif // CHATDIALOG_H
