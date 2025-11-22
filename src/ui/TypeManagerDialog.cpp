#include "TypeManagerDialog.h"
#include "ui_TypeManagerDialog.h"
#include <QMessageBox>

TypeManagerDialog::TypeManagerDialog(UserDataManager *userDataMgr, QWidget *parent)
    : QDialog(parent), ui(new Ui::TypeManagerDialog), userDataManager(userDataMgr)
{
    ui->setupUi(this);

    // 连接信号槽
    connect(ui->addButton, &QPushButton::clicked, this, &TypeManagerDialog::onAddType);
    connect(ui->deleteButton, &QPushButton::clicked, this, &TypeManagerDialog::onDeleteType);
    connect(ui->newTypeEdit, &QLineEdit::returnPressed, this, &TypeManagerDialog::onAddType);

    // 加载类型列表
    loadTypes();
}

TypeManagerDialog::~TypeManagerDialog()
{
    delete ui;
}

void TypeManagerDialog::loadTypes()
{
    ui->typesList->clear();

    QStringList types = userDataManager->getAllTypes();
    for (const QString &type : types)
    {
        ui->typesList->addItem(type);
    }
}

void TypeManagerDialog::onAddType()
{
    QString newType = ui->newTypeEdit->text().trimmed();

    if (newType.isEmpty())
    {
        QMessageBox::warning(this, "输入错误", "类型名称不能为空");
        return;
    }

    // 检查是否已存在
    if (userDataManager->hasType(newType))
    {
        QMessageBox::warning(this, "类型已存在", QString("类型 \"%1\" 已经存在").arg(newType));
        return;
    }

    // 添加类型
    if (userDataManager->addType(newType))
    {
        ui->newTypeEdit->clear();
        loadTypes();
        QMessageBox::information(this, "添加成功", QString("已添加类型: %1").arg(newType));
    }
    else
    {
        QMessageBox::warning(this, "添加失败", "无法添加类型");
    }
}

void TypeManagerDialog::onDeleteType()
{
    QListWidgetItem *currentItem = ui->typesList->currentItem();

    if (!currentItem)
    {
        QMessageBox::warning(this, "未选择", "请先选择要删除的类型");
        return;
    }

    QString typeName = currentItem->text();

    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        QString("确定要删除类型 \"%1\" 吗？").arg(typeName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (userDataManager->removeType(typeName))
        {
            loadTypes();
            QMessageBox::information(this, "删除成功", QString("已删除类型: %1").arg(typeName));
        }
        else
        {
            QMessageBox::warning(this, "删除失败", "无法删除类型");
        }
    }
}
