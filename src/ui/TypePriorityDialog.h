#ifndef TYPEPRIORITYDIALOG_H
#define TYPEPRIORITYDIALOG_H

#include "../data/UserDataManager.h"
#include <QDialog>

namespace Ui
{
    class TypePriorityDialog;
}

/**
 * @brief 类型优先级设置对话框
 *
 * 允许用户设置 Mod 类型的排序优先级
 * 用于 ModSorter 排序时的类型优先级参考
 */
class TypePriorityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TypePriorityDialog(UserDataManager *userDataMgr, QWidget *parent = nullptr);
    ~TypePriorityDialog();

protected:
    void accept() override;

private slots:
    void onResetToDefault();

private:
    Ui::TypePriorityDialog *ui;
    UserDataManager *userDataManager;

    void loadTypePriority();
    void saveTypePriority();
    QStringList getCurrentPriority() const;
};

#endif // TYPEPRIORITYDIALOG_H
