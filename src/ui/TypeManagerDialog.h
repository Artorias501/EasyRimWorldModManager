#ifndef TYPEMANAGERDIALOG_H
#define TYPEMANAGERDIALOG_H

#include "../data/UserDataManager.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class TypeManagerDialog;
}
QT_END_NAMESPACE

class TypeManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TypeManagerDialog(UserDataManager *userDataMgr, QWidget *parent = nullptr);
    ~TypeManagerDialog();

private slots:
    void onAddType();
    void onDeleteType();

private:
    Ui::TypeManagerDialog *ui;
    UserDataManager *userDataManager;

    void loadTypes();
    void loadDefaultTypes();
    void loadCustomTypes();
};

#endif // TYPEMANAGERDIALOG_H
