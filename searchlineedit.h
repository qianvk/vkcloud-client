#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>

class QListWidgetItem;
class ListItemBase;
using InstantSearchRets = std::vector<std::pair<QListWidgetItem*, ListItemBase*>>;
class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = nullptr);
    void SetMaxLength(int max_len);

signals:
    void SigShowSearchList(const QPoint &relative_pos, int width, QWidget *parent);

protected:
    void focusOutEvent(QFocusEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    int max_len_;

private slots:
    void LimitTextLength(const QString &text);
    void SlotInstantSearch(const QString &text);
signals:
    void SigFocusOut();
    void SigInstantSearchRets(const InstantSearchRets& search_rets);
};

#endif // SEARCHLINEEDIT_H
