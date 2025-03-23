#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = nullptr);
    void SetMaxLength(int max_len);

protected:
    void focusOutEvent(QFocusEvent *event) override;

private:
    int max_len_;

private slots:
    void LimitTextLength(QString text);
signals:
    void SigFocusOut();
};

#endif // SEARCHLINEEDIT_H
