#ifndef MODMANAGER_H
#define MODMANAGER_H

#include "ModItem.h"
#include "OfficialDLCScanner.h"
#include "WorkshopScanner.h"
#include <QList>
#include <QMap>
#include <QString>

/**
 * @brief Mod管理器
 *
 * 统一管理所有Mod，包括Steam创意工坊Mod和官方DLC
 */
class ModManager
{
public:
    ModManager();

    explicit ModManager(const QString &steamPath);

    ~ModManager();

    // 设置Steam路径
    void setSteamPath(const QString &steamPath);
    QString getSteamPath() const { return m_steamPath; }

    // 设置游戏安装路径
    void setGameInstallPath(const QString &gameInstallPath);
    QString getGameInstallPath() const { return m_gameInstallPath; }

    // 扫描所有Mod（包括工坊mod和官方DLC）
    bool scanAll();

    // 单独扫描
    bool scanWorkshopMods();

    bool scanOfficialDLCs();

    // 获取所有Mod
    QList<ModItem *> getAllMods() const;

    // 获取工坊Mod
    QList<ModItem *> getWorkshopMods() const;

    // 获取官方DLC
    QList<ModItem *> getOfficialDLCs() const;

    // 根据PackageId查找Mod
    ModItem *findModByPackageId(const QString &packageId) const;

    // 检查Mod是否为官方DLC
    bool isOfficialDLC(const QString &packageId) const;

    // 清除所有数据
    void clear();

private:
    QString m_steamPath;                     // Steam安装路径
    QString m_gameInstallPath;               // 游戏安装路径
    WorkshopScanner *m_workshopScanner;      // 工坊扫描器
    OfficialDLCScanner *m_dlcScanner;        // DLC扫描器
    QMap<QString, ModItem *> m_packageIdMap; // PackageId到Mod的统一映射
};

#endif // MODMANAGER_H
