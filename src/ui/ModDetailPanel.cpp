#include "ModDetailPanel.h"
#include "ui_ModDetailPanel.h"

ModDetailPanel::ModDetailPanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::ModDetailPanel), currentMod(nullptr), modManager(nullptr)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, this, &ModDetailPanel::onSaveClicked);

    clearDisplay();
}

ModDetailPanel::~ModDetailPanel()
{
    delete ui;
}

void ModDetailPanel::setModItem(ModItem *mod, ModManager *manager)
{
    currentMod = mod;
    modManager = manager;

    if (!mod)
    {
        clearDisplay();
        return;
    }

    loadModTypes();
    displayModInfo();
    displayDependencies();
}

void ModDetailPanel::clearDisplay()
{
    ui->nameValue->setText("-");
    ui->packageIdValue->setText("-");
    ui->authorValue->setText("-");
    ui->versionValue->setText("-");
    ui->typeComboBox->clear();
    ui->remarkTextEdit->clear();
    ui->descriptionBrowser->clear();
    ui->dependenciesList->clear();
    ui->loadBeforeList->clear();
    ui->loadAfterList->clear();
    ui->incompatibleList->clear();

    ui->saveButton->setEnabled(false);
}

void ModDetailPanel::loadModTypes()
{
    ui->typeComboBox->clear();

    if (!modManager)
    {
        return;
    }

    UserDataManager *userDataMgr = modManager->getUserDataManager();
    QStringList types = userDataMgr->getAllTypes();

    ui->typeComboBox->addItem("未分类", "");
    for (const QString &type : types)
    {
        ui->typeComboBox->addItem(type, type);
    }
}

void ModDetailPanel::refreshTypeComboBox()
{
    if (!currentMod)
    {
        return;
    }

    QString currentType = currentMod->type;
    loadModTypes();

    // 恢复选中的类型
    int index = ui->typeComboBox->findData(currentType);
    if (index >= 0)
    {
        ui->typeComboBox->setCurrentIndex(index);
    }
}

void ModDetailPanel::displayModInfo()
{
    if (!currentMod)
    {
        return;
    }

    // 基本信息
    ui->nameValue->setText(currentMod->name.isEmpty() ? "-" : currentMod->name);
    ui->packageIdValue->setText(currentMod->packageId.isEmpty() ? "-" : currentMod->packageId);
    ui->authorValue->setText(currentMod->author.isEmpty() ? "-" : currentMod->author);

    // 支持版本
    if (currentMod->supportedVersions.isEmpty())
    {
        ui->versionValue->setText("-");
    }
    else
    {
        ui->versionValue->setText(currentMod->supportedVersions.join(", "));
    }

    // 可编辑信息
    int typeIndex = ui->typeComboBox->findData(currentMod->type);
    if (typeIndex >= 0)
    {
        ui->typeComboBox->setCurrentIndex(typeIndex);
    }
    else
    {
        ui->typeComboBox->setCurrentIndex(0); // 未分类
    }

    ui->remarkTextEdit->setText(currentMod->remark);

    // 描述
    ui->descriptionBrowser->setPlainText(currentMod->description.isEmpty() ? "无描述" : currentMod->description);

    // 如果有URL，添加链接
    if (!currentMod->url.isEmpty())
    {
        ui->descriptionBrowser->append(QString("\n<a href=\"%1\">查看详情</a>").arg(currentMod->url));
    }

    ui->saveButton->setEnabled(true);
}

void ModDetailPanel::displayDependencies()
{
    if (!currentMod)
    {
        return;
    }

    // 依赖
    ui->dependenciesList->clear();
    if (currentMod->dependencies.isEmpty())
    {
        ui->dependenciesList->addItem("(无)");
    }
    else
    {
        for (const QString &dep : currentMod->dependencies)
        {
            ui->dependenciesList->addItem(dep);
        }
    }

    // 应在之前加载
    ui->loadBeforeList->clear();
    if (currentMod->loadBefore.isEmpty())
    {
        ui->loadBeforeList->addItem("(无)");
    }
    else
    {
        for (const QString &mod : currentMod->loadBefore)
        {
            ui->loadBeforeList->addItem(mod);
        }
    }

    // 应在之后加载
    ui->loadAfterList->clear();
    if (currentMod->loadAfter.isEmpty())
    {
        ui->loadAfterList->addItem("(无)");
    }
    else
    {
        for (const QString &mod : currentMod->loadAfter)
        {
            ui->loadAfterList->addItem(mod);
        }
    }

    // 不兼容
    ui->incompatibleList->clear();
    if (currentMod->incompatibleWith.isEmpty())
    {
        ui->incompatibleList->addItem("(无)");
    }
    else
    {
        for (const QString &mod : currentMod->incompatibleWith)
        {
            ui->incompatibleList->addItem(mod);
        }
    }
}

void ModDetailPanel::onSaveClicked()
{
    if (!currentMod || !modManager)
    {
        return;
    }

    // 保存类型
    QString selectedType = ui->typeComboBox->currentData().toString();
    currentMod->type = selectedType;

    // 保存备注
    currentMod->remark = ui->remarkTextEdit->toPlainText();

    // 通知父窗口保存数据
    emit modDetailsChanged();
}
