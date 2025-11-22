#include "TypePriorityDialog.h"
#include "ui_TypePriorityDialog.h"
#include <QMessageBox>

TypePriorityDialog::TypePriorityDialog(UserDataManager *userDataMgr, QWidget *parent)
    : QDialog(parent), ui(new Ui::TypePriorityDialog), userDataManager(userDataMgr)
{
    ui->setupUi(this);

    connect(ui->resetButton, &QPushButton::clicked, this, &TypePriorityDialog::onResetToDefault);

    loadTypePriority();
}

TypePriorityDialog::~TypePriorityDialog()
{
    delete ui;
}

void TypePriorityDialog::loadTypePriority()
{
    ui->priorityList->clear();

    // 获取当前的优先级列表
    QStringList priority = userDataManager->getTypePriority();

    // 获取所有可用类型
    QStringList allTypes = userDataManager->getAllTypes();

    // 如果没有设置优先级，使用默认顺序（默认类型列表）
    if (priority.isEmpty())
    {
        priority = UserDataManager::getDefaultTypes();
    }

    // 先添加已设置优先级的类型
    for (const QString &type : priority)
    {
        if (allTypes.contains(type))
        {
            QListWidgetItem *item = new QListWidgetItem(type);
            ui->priorityList->addItem(item);
        }
    }

    // 添加未设置优先级的类型（放在最后）
    for (const QString &type : allTypes)
    {
        if (!priority.contains(type))
        {
            QListWidgetItem *item = new QListWidgetItem(type);
            item->setForeground(QColor(128, 128, 128)); // 灰色显示未设置的
            ui->priorityList->addItem(item);
        }
    }
}

void TypePriorityDialog::accept()
{
    saveTypePriority();
    QDialog::accept();
}

void TypePriorityDialog::saveTypePriority()
{
    QStringList priority = getCurrentPriority();
    userDataManager->setTypePriority(priority);
    userDataManager->saveTypePriority();
}

QStringList TypePriorityDialog::getCurrentPriority() const
{
    QStringList priority;
    for (int i = 0; i < ui->priorityList->count(); ++i)
    {
        QListWidgetItem *item = ui->priorityList->item(i);
        priority.append(item->text());
    }
    return priority;
}

void TypePriorityDialog::onResetToDefault()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认重置",
        "是否重置为默认的类型优先级顺序？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // 清空列表
        ui->priorityList->clear();

        // 添加默认类型顺序
        QStringList defaultTypes = UserDataManager::getDefaultTypes();
        for (const QString &type : defaultTypes)
        {
            QListWidgetItem *item = new QListWidgetItem(type);
            ui->priorityList->addItem(item);
        }

        // 添加自定义类型（如果有的话）
        QStringList customTypes = userDataManager->getAllCustomTypes();
        for (const QString &type : customTypes)
        {
            QListWidgetItem *item = new QListWidgetItem(type);
            item->setForeground(QColor(128, 128, 128));
            ui->priorityList->addItem(item);
        }
    }
}
