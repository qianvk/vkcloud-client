#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

#include "global.h"

namespace Ui {
class ChatDialog;
}

class QListWidgetItem;
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

private:
    bool _b_loading;
    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
};

#endif // CHATDIALOG_H
