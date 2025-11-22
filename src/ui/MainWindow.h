#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../data/ModConfigManager.h"
#include "../data/ModManager.h"
#include "../data/PathConfig.h"
#include "../data/UserDataManager.h"
#include <QListWidgetItem>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class ModDetailPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void startScan();

private slots:
    // 菜单栏操作
    void onSaveToGame();
    void onSaveAs();
    void onLoadConfig();
    void onManageTypes();
    void onRescan();
    void onPathSettings();
    void onTypePriority();
    void onAutoSort();
    void onAbout();

    // Mod列表操作
    void onAddSelected();
    void onRemoveMod();
    void onMoveUp();
    void onMoveDown();

    // 列表选择变化
    void onUnloadedModSelected(QListWidgetItem *item);
    void onLoadedModSelected(QListWidgetItem *item);

    // 搜索过滤
    void onUnloadedSearchChanged(const QString &text);
    void onLoadedSearchChanged(const QString &text);

    // 详情面板更新
    void onModDetailChanged();

    // 拖拽排序完成
    void onLoadedListOrderChanged();

private:
    Ui::MainWindow *ui;
    ModManager *modManager;
    ModConfigManager *configManager;
    ModDetailPanel *detailPanel;
    PathConfig pathConfig;

    ModItem *currentSelectedMod;

    // 初始化
    void setupConnections();
    void initializeManagers();
    void loadGameConfig();

    // UI更新
    void updateModLists();
    void updateUnloadedList();
    void updateLoadedList();
    void filterUnloadedList(const QString &filter);
    void filterLoadedList(const QString &filter);

    // 列表项创建
    QListWidgetItem *createModListItem(ModItem *mod);
    QListWidgetItem *createLoadedModListItem(ModItem *mod);
    QString getModDisplayText(ModItem *mod);

    // 依赖检查
    bool checkModDependencies(ModItem *mod, QStringList &missingDeps);
    bool checkModLoadOrder(ModItem *mod, QStringList &orderIssues);
    QStringList checkDependentMods(const QString &packageId);
    void updateLoadedListWithDependencyCheck();

    // 状态查询
    bool isModLoaded(const QString &packageId);
    ModItem *getModByPackageId(const QString &packageId);

    // 配置操作
    void saveCurrentConfig();
    void loadConfigFromFile(const QString &filePath);

    // 状态栏消息
    void showStatusMessage(const QString &message, int timeout = 3000);
};

#endif // MAINWINDOW_H
