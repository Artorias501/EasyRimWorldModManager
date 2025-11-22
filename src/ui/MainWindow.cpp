#include "MainWindow.h"
#include "../data/WorkshopScanner.h"
#include "ModDetailPanel.h"
#include "PathSettingsDialog.h"
#include "TypeManagerDialog.h"
#include "TypePriorityDialog.h"
#include "../data/ModSorter.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), modManager(nullptr), configManager(nullptr), detailPanel(nullptr),
      currentSelectedMod(nullptr) {
    ui->setupUi(this);

    // 加载路径配置
    pathConfig.load();

    // 创建详情面板
    detailPanel = new ModDetailPanel(this);
    ui->scrollArea->setWidget(detailPanel);

    // 设置splitter的初始比例 (1:1:1)
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setStretchFactor(2, 3);

    // 初始化管理器
    initializeManagers();

    // 连接信号槽
    setupConnections();

    showStatusMessage("就绪");
}

MainWindow::~MainWindow() {
    delete ui;
    if (modManager) {
        delete modManager;
    }
    if (configManager) {
        delete configManager;
    }
}

void MainWindow::setupConnections() {
    // 菜单栏
    connect(ui->actionSaveToGame, &QAction::triggered, this, &MainWindow::onSaveToGame);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAs);
    connect(ui->actionLoadConfig, &QAction::triggered, this, &MainWindow::onLoadConfig);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionManageTypes, &QAction::triggered, this, &MainWindow::onManageTypes);
    connect(ui->actionRescan, &QAction::triggered, this, &MainWindow::onRescan);
    connect(ui->actionPathSettings, &QAction::triggered, this, &MainWindow::onPathSettings);
    connect(ui->actionTypePriority, &QAction::triggered, this, &MainWindow::onTypePriority);
    connect(ui->actionAutoSort, &QAction::triggered, this, &MainWindow::onAutoSort);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    // 按钮
    connect(ui->addSelectedButton, &QPushButton::clicked, this, &MainWindow::onAddSelected);
    connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveMod);
    connect(ui->moveUpButton, &QPushButton::clicked, this, &MainWindow::onMoveUp);
    connect(ui->moveDownButton, &QPushButton::clicked, this, &MainWindow::onMoveDown);

    // 列表选择
    connect(ui->unloadedModsList, &QListWidget::itemClicked, this, &MainWindow::onUnloadedModSelected);
    connect(ui->loadedModsList, &QListWidget::itemClicked, this, &MainWindow::onLoadedModSelected);

    // 搜索框
    connect(ui->unloadedSearchEdit, &QLineEdit::textChanged, this, &MainWindow::onUnloadedSearchChanged);
    connect(ui->loadedSearchEdit, &QLineEdit::textChanged, this, &MainWindow::onLoadedSearchChanged);

    // 拖拽排序
    connect(ui->loadedModsList->model(), &QAbstractItemModel::rowsMoved,
            this, &MainWindow::onLoadedListOrderChanged);

    // 详情面板
    connect(detailPanel, &ModDetailPanel::modDetailsChanged, this, &MainWindow::onModDetailChanged);
}

void MainWindow::initializeManagers() {
    modManager = new ModManager();

    // 使用路径配置初始化 ModManager
    if (pathConfig.isValid()) {
        modManager->setSteamPath(pathConfig.getSteamPath());
        modManager->setGameInstallPath(pathConfig.getGameInstallPath());
    }

    // 使用用户存档路径初始化 ModConfigManager
    if (!pathConfig.getUserSavePath().isEmpty()) {
        QString configPath = QDir(pathConfig.getUserSavePath()).absoluteFilePath("Config/ModsConfig.xml");
        configManager = new ModConfigManager(configPath);
    } else {
        configManager = new ModConfigManager();
    }

    showStatusMessage("初始化中...");
}

void MainWindow::startScan() {
    // 显示进度对话框
    QProgressDialog *progressDialog = new QProgressDialog("正在扫描 Mod...", "取消", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);
    progressDialog->setValue(0);

    // 创建 Future Watcher
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressDialog]() {
        progressDialog->close();

        bool success = watcher->result();
        if (success) {
            loadGameConfig();
            updateModLists();
            showStatusMessage(QString("扫描完成，共找到 %1 个 Mod").arg(modManager->getAllMods().count()));
        } else {
            QMessageBox::warning(this, "扫描失败", "无法扫描 Mod，请检查游戏路径设置");
            showStatusMessage("扫描失败");
        }

        watcher->deleteLater();
        progressDialog->deleteLater();
    });

    // 在后台线程执行扫描
    QFuture<bool> future = QtConcurrent::run([this]() { return modManager->scanAll(); });

    watcher->setFuture(future);
}

void MainWindow::loadGameConfig() {
    if (!configManager->loadConfig()) {
        showStatusMessage("无法加载游戏配置");
    }
}

void MainWindow::updateModLists() {
    updateUnloadedList();
    updateLoadedList();
}

void MainWindow::updateUnloadedList() {
    ui->unloadedModsList->clear();

    QStringList loadedMods = configManager->getActiveMods();
    QList<ModItem *> allMods = modManager->getAllMods();

    for (ModItem *mod: allMods) {
        if (!isModLoaded(mod->packageId)) {
            QListWidgetItem *item = createModListItem(mod);
            ui->unloadedModsList->addItem(item);
        }
    }
}

void MainWindow::updateLoadedList() {
    ui->loadedModsList->clear();

    QStringList activeMods = configManager->getActiveMods();

    for (const QString &packageId: activeMods) {
        ModItem *mod = getModByPackageId(packageId);
        if (mod) {
            QListWidgetItem *item = createModListItem(mod);
            ui->loadedModsList->addItem(item);
        }
    }
}

QListWidgetItem *MainWindow::createModListItem(ModItem *mod) {
    QString displayText = getModDisplayText(mod);
    QString typeText = mod->type.isEmpty() ? "未分类" : mod->type;

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(QString("%1\n[%2]").arg(displayText, typeText));
    item->setData(Qt::UserRole, mod->packageId);

    // 根据类型设置不同的颜色
    if (mod->isOfficialDLC) {
        item->setForeground(QColor(0, 120, 215)); // 蓝色
    } else if (mod->packageId.toLower() == "ludeon.rimworld") {
        item->setForeground(QColor(0, 150, 0)); // 绿色 - Core
    }

    return item;
}

QString MainWindow::getModDisplayText(ModItem *mod) {
    if (!mod->remark.isEmpty()) {
        return mod->remark;
    }
    if (!mod->name.isEmpty()) {
        return mod->name;
    }
    return mod->packageId;
}

bool MainWindow::isModLoaded(const QString &packageId) {
    QStringList activeMods = configManager->getActiveMods();
    for (const QString &activeId: activeMods) {
        if (activeId.compare(packageId, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

ModItem *MainWindow::getModByPackageId(const QString &packageId) {
    return modManager->findModByPackageId(packageId);
}

void MainWindow::onSaveToGame() {
    if (configManager->saveConfig()) {
        QMessageBox::information(this, "保存成功", "已成功保存到游戏配置文件");
        showStatusMessage("已保存到游戏配置");
    } else {
        QMessageBox::warning(this, "保存失败", "无法保存到游戏配置文件");
        showStatusMessage("保存失败");
    }
}

void MainWindow::onSaveAs() {
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "另存为 Mod 配置",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/ModConfig.xml",
        "XML 文件 (*.xml)");

    if (!filePath.isEmpty()) {
        UserDataManager *userMgr = modManager->getUserDataManager();
        QStringList activeMods = configManager->getActiveMods();
        QStringList knownExpansions = configManager->getKnownExpansions();
        QString version = configManager->getVersion();

        if (userMgr->saveModListToPath(filePath, activeMods, knownExpansions, version)) {
            QMessageBox::information(this, "保存成功", QString("已保存到: %1").arg(filePath));
            showStatusMessage("配置已另存");
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存配置文件");
        }
    }
}

void MainWindow::onLoadConfig() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "加载 Mod 配置",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "XML 文件 (*.xml)");

    if (!filePath.isEmpty()) {
        loadConfigFromFile(filePath);
    }
}

void MainWindow::loadConfigFromFile(const QString &filePath) {
    if (configManager->loadConfig(filePath)) {
        updateModLists();
        showStatusMessage("配置已加载");
    } else {
        QMessageBox::warning(this, "加载失败", "无法加载配置文件");
    }
}

void MainWindow::onManageTypes() {
    TypeManagerDialog dialog(modManager->getUserDataManager(), this);
    if (dialog.exec() == QDialog::Accepted) {
        // 刷新详情面板的类型下拉框
        detailPanel->refreshTypeComboBox();
        // 重新加载用户数据
        modManager->getUserDataManager()->loadAll();
        // 更新列表显示
        updateModLists();
        showStatusMessage("类型已更新");
    }
}

void MainWindow::onRescan() {
    startScan();
}

void MainWindow::onPathSettings() {
    PathSettingsDialog dialog(&pathConfig, this);

    if (dialog.exec() == QDialog::Accepted) {
        // 获取新配置
        PathConfig newConfig = dialog.getPathConfig();

        // 保存配置
        if (newConfig.save()) {
            QMessageBox::information(this, "设置已保存",
                                     "路径设置已保存。\n\n"
                                     "需要重启程序才能使新设置生效。\n\n"
                                     "是否现在重启？",
                                     QMessageBox::Yes | QMessageBox::No);

            // 提示用户重启
            QMessageBox restartMsg;
            restartMsg.setWindowTitle("需要重启");
            restartMsg.setText("路径设置已保存，需要重启程序才能生效。");
            restartMsg.setInformativeText("请手动关闭并重新启动程序。");
            restartMsg.setIcon(QMessageBox::Information);
            restartMsg.setStandardButtons(QMessageBox::Ok);
            restartMsg.exec();
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存路径配置");
        }
    }
}

void MainWindow::onTypePriority() {
    TypePriorityDialog dialog(modManager->getUserDataManager(), this);

    if (dialog.exec() == QDialog::Accepted) {
        showStatusMessage("类型优先级已更新");
    }
}

void MainWindow::onAutoSort() {
    // 获取当前已加载的 Mod 列表
    QStringList activeMods = configManager->getActiveMods();
    if (activeMods.isEmpty()) {
        QMessageBox::information(this, "排序", "当前没有已加载的 Mod");
        return;
    }

    // 构建 ModItem 列表
    QList<ModItem *> modsToSort;
    for (const QString &packageId: activeMods) {
        ModItem *mod = modManager->findModByPackageId(packageId);
        if (mod) {
            modsToSort.append(mod);
        }
    }

    if (modsToSort.isEmpty()) {
        QMessageBox::warning(this, "排序", "找不到已加载的 Mod 信息");
        return;
    }

    // 检查循环依赖
    if (ModSorter::hasCircularDependency(modsToSort)) {
        QStringList circular = ModSorter::getCircularDependencies(modsToSort);
        QString circularInfo = circular.join("\n");

        QMessageBox::warning(this, "检测到循环依赖",
                             QString("以下 Mod 存在循环依赖，可能无法完全排序：\n\n%1\n\n"
                                 "请检查 Mod 的依赖设置。")
                             .arg(circularInfo));
    }

    // 获取类型优先级
    QStringList typePriority = modManager->getUserDataManager()->getTypePriority();

    // 执行排序
    QList<ModItem *> sorted = ModSorter::sortMods(modsToSort, typePriority);

    // 更新 configManager
    QStringList sortedIds;
    for (ModItem *mod: sorted) {
        sortedIds.append(mod->packageId);
    }
    configManager->setActiveMods(sortedIds);

    // 更新 UI
    updateLoadedList();

    showStatusMessage(QString("排序完成，共 %1 个 Mod").arg(sorted.size()));

    QMessageBox::information(this, "排序完成",
                             QString("已按照依赖关系和类型优先级重新排序 %1 个 Mod。\n\n"
                                 "请检查排序结果是否符合预期。")
                             .arg(sorted.size()));
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "关于",
                       "环世界 Mod 管理器\n\n"
                       "版本: 1.0\n\n"
                       "用于管理 RimWorld 游戏的 Mod 加载顺序和配置\n\n"
                       "© 2025");
}

void MainWindow::onAddSelected() {
    QList<QListWidgetItem *> selectedItems = ui->unloadedModsList->selectedItems();

    if (selectedItems.isEmpty()) {
        showStatusMessage("请先选择要添加的 Mod");
        return;
    }

    for (QListWidgetItem *item: selectedItems) {
        QString packageId = item->data(Qt::UserRole).toString();
        configManager->addMod(packageId);
    }

    updateModLists();
    showStatusMessage(QString("已添加 %1 个 Mod").arg(selectedItems.count()));
}

void MainWindow::onRemoveMod() {
    QListWidgetItem *currentItem = ui->loadedModsList->currentItem();
    if (!currentItem) {
        showStatusMessage("请先选择要移除的 Mod");
        return;
    }

    QString packageId = currentItem->data(Qt::UserRole).toString();

    // 检查是否是 Core
    if (packageId.compare("ludeon.rimworld", Qt::CaseInsensitive) == 0) {
        QMessageBox::warning(this, "无法移除", "Core 是必需的，不能移除");
        return;
    }

    configManager->removeMod(packageId);
    updateModLists();
    showStatusMessage("Mod 已移除");
}

void MainWindow::onMoveUp() {
    int currentRow = ui->loadedModsList->currentRow();
    if (currentRow <= 0) {
        return;
    }

    QListWidgetItem *currentItem = ui->loadedModsList->currentItem();
    QString packageId = currentItem->data(Qt::UserRole).toString();

    if (configManager->moveModUp(packageId)) {
        updateLoadedList();
        ui->loadedModsList->setCurrentRow(currentRow - 1);
        showStatusMessage("已上移");
    }
}

void MainWindow::onMoveDown() {
    int currentRow = ui->loadedModsList->currentRow();
    if (currentRow < 0 || currentRow >= ui->loadedModsList->count() - 1) {
        return;
    }

    QListWidgetItem *currentItem = ui->loadedModsList->currentItem();
    QString packageId = currentItem->data(Qt::UserRole).toString();

    if (configManager->moveModDown(packageId)) {
        updateLoadedList();
        ui->loadedModsList->setCurrentRow(currentRow + 1);
        showStatusMessage("已下移");
    }
}

void MainWindow::onUnloadedModSelected(QListWidgetItem *item) {
    if (!item)
        return;

    QString packageId = item->data(Qt::UserRole).toString();
    currentSelectedMod = getModByPackageId(packageId);

    if (currentSelectedMod) {
        detailPanel->setModItem(currentSelectedMod, modManager);
    }
}

void MainWindow::onLoadedModSelected(QListWidgetItem *item) {
    if (!item)
        return;

    QString packageId = item->data(Qt::UserRole).toString();
    currentSelectedMod = getModByPackageId(packageId);

    if (currentSelectedMod) {
        detailPanel->setModItem(currentSelectedMod, modManager);
    }
}

void MainWindow::onUnloadedSearchChanged(const QString &text) {
    filterUnloadedList(text);
}

void MainWindow::onLoadedSearchChanged(const QString &text) {
    filterLoadedList(text);
}

void MainWindow::filterUnloadedList(const QString &filter) {
    for (int i = 0; i < ui->unloadedModsList->count(); ++i) {
        QListWidgetItem *item = ui->unloadedModsList->item(i);
        QString packageId = item->data(Qt::UserRole).toString();
        ModItem *mod = getModByPackageId(packageId);

        if (filter.isEmpty()) {
            item->setHidden(false);
        } else {
            bool matches = false;
            QString lowerFilter = filter.toLower();

            if (mod) {
                matches = mod->name.toLower().contains(lowerFilter) ||
                          mod->packageId.toLower().contains(lowerFilter) ||
                          mod->remark.toLower().contains(lowerFilter) ||
                          mod->type.toLower().contains(lowerFilter);
            }

            item->setHidden(!matches);
        }
    }
}

void MainWindow::filterLoadedList(const QString &filter) {
    for (int i = 0; i < ui->loadedModsList->count(); ++i) {
        QListWidgetItem *item = ui->loadedModsList->item(i);
        QString packageId = item->data(Qt::UserRole).toString();
        ModItem *mod = getModByPackageId(packageId);

        if (filter.isEmpty()) {
            item->setHidden(false);
        } else {
            bool matches = false;
            QString lowerFilter = filter.toLower();

            if (mod) {
                matches = mod->name.toLower().contains(lowerFilter) ||
                          mod->packageId.toLower().contains(lowerFilter) ||
                          mod->remark.toLower().contains(lowerFilter) ||
                          mod->type.toLower().contains(lowerFilter);
            }

            item->setHidden(!matches);
        }
    }
}

void MainWindow::onModDetailChanged() {
    // 保存修改
    modManager->saveModsToUserData();

    // 更新列表显示
    updateModLists();

    showStatusMessage("Mod 详情已更新");
}

void MainWindow::onLoadedListOrderChanged() {
    // 用户通过拖拽改变了顺序，需要更新 configManager
    QStringList newOrder;

    for (int i = 0; i < ui->loadedModsList->count(); ++i) {
        QListWidgetItem *item = ui->loadedModsList->item(i);
        QString packageId = item->data(Qt::UserRole).toString();
        newOrder.append(packageId);
    }

    configManager->setActiveMods(newOrder);
    showStatusMessage("加载顺序已更新");
}

void MainWindow::showStatusMessage(const QString &message, int timeout) {
    ui->statusbar->showMessage(message, timeout);
}
