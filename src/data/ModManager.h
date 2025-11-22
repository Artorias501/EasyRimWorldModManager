#ifndef MODMANAGER_H
#define MODMANAGER_H

#include "ModItem.h"
#include "OfficialDLCScanner.h"
#include "UserDataManager.h"
#include "WorkshopScanner.h"
#include <QList>
#include <QMap>
#include <QString>

/**
 * @brief Mod管理器（中心管理类）
 *
 * 统一管理所有Mod，包括Steam创意工坊Mod和官方DLC
 * - 缓存所有扫描的Mod和DLC
 * - 整合UserDataManager，自动同步用户的备注和类型数据
 * - OfficialDLCScanner和WorkshopScanner仅为ModManager的私有服务
 */
class ModManager
{
public:
    ModManager();

    explicit ModManager(const QString &steamPath);

    ~ModManager();

    // ==================== 路径管理 ====================

    // 设置Steam路径
    void setSteamPath(const QString &steamPath);
    QString getSteamPath() const { return m_steamPath; }

    // 设置游戏安装路径
    void setGameInstallPath(const QString &gameInstallPath);
    QString getGameInstallPath() const { return m_gameInstallPath; }

    // ==================== Mod扫描和缓存 ====================

    // 扫描所有Mod（包括工坊mod和官方DLC）
    // 扫描后自动从UserDataManager加载备注和类型，并更新到ModItem
    bool scanAll();

    // 单独扫描工坊Mod
    bool scanWorkshopMods();

    // 单独扫描官方DLC
    bool scanOfficialDLCs();

    // ==================== 缓存数据访问 ====================

    // 获取所有Mod（从缓存）
    QList<ModItem *> getAllMods() const;

    // 获取工坊Mod（从缓存）
    QList<ModItem *> getWorkshopMods() const;

    // 获取官方DLC（从缓存）
    QList<ModItem *> getOfficialDLCs() const;

    // 根据PackageId查找Mod（从缓存）
    ModItem *findModByPackageId(const QString &packageId) const;

    // 检查Mod是否为官方DLC
    bool isOfficialDLC(const QString &packageId) const;

    // ==================== UserDataManager联动 ====================

    // 获取UserDataManager（用于外部访问用户数据）
    UserDataManager *getUserDataManager() { return m_userDataManager; }
    const UserDataManager *getUserDataManager() const { return m_userDataManager; }

    // 从UserDataManager加载备注和类型到所有缓存的ModItem
    void loadUserDataToMods();

    // 将所有缓存的ModItem的备注和类型保存到UserDataManager
    void saveModsToUserData();

    // ==================== 清理 ====================

    // 清除所有数据
    void clear();

private:
    // 路径
    QString m_steamPath;       // Steam安装路径
    QString m_gameInstallPath; // 游戏安装路径

    // 扫描器（私有服务，仅供ModManager使用）
    WorkshopScanner *m_workshopScanner; // 工坊扫描器
    OfficialDLCScanner *m_dlcScanner;   // DLC扫描器

    // 用户数据管理器
    UserDataManager *m_userDataManager; // 用户数据管理器

    // 缓存数据
    QList<ModItem *> m_cachedWorkshopMods;   // 缓存的工坊Mod列表
    QList<ModItem *> m_cachedOfficialDLCs;   // 缓存的官方DLC列表
    QMap<QString, ModItem *> m_packageIdMap; // PackageId到Mod的统一映射
};

#endif // MODMANAGER_H
