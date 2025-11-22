#ifndef WORKSHOPSCANNER_H
#define WORKSHOPSCANNER_H

#include "ModItem.h"
#include <QList>
#include <QMap>
#include <QString>
#include <QXmlStreamReader>

/**
 * @brief Steam创意工坊Mod扫描器
 *
 * 负责扫描Steam创意工坊目录，读取Mod的About.xml文件
 * Steam工坊路径: {Steam安装路径}\steamapps\workshop\content\294100
 * 每个Mod目录包含: About\About.xml
 */
class WorkshopScanner
{
public:
    WorkshopScanner();

    explicit WorkshopScanner(const QString &workshopPath);

    // 设置Steam创意工坊路径
    void setWorkshopPath(const QString &path);

    QString getWorkshopPath() const { return m_workshopPath; }

    // 扫描所有Mod
    bool scanAllMods();

    // 获取扫描到的Mod列表
    QList<ModItem *> getScannedMods() const { return m_scannedMods; }

    // 根据PackageId查找Mod
    ModItem *findModByPackageId(const QString &packageId) const;

    // 根据WorkshopId查找Mod
    ModItem *findModByWorkshopId(const QString &workshopId) const;

    // 清除扫描结果
    void clear();

    // 自动检测Steam安装路径
    static QString detectSteamPath();

    // 获取默认工坊路径
    static QString getDefaultWorkshopPath();

private:
    QString m_workshopPath;                   // Steam创意工坊路径
    QList<ModItem *> m_scannedMods;           // 扫描到的Mod列表
    QMap<QString, ModItem *> m_packageIdMap;  // PackageId到Mod的映射
    QMap<QString, ModItem *> m_workshopIdMap; // WorkshopId到Mod的映射

    // 扫描单个Mod目录
    ModItem *scanModDirectory(const QString &modDirPath, const QString &workshopId);

    // 解析About.xml文件
    bool parseAboutXml(ModItem *mod, const QString &aboutXmlPath);

    // XML 解析辅助方法
    void parseSupportedVersions(QXmlStreamReader &xml, ModItem *mod);
    void parseModDependencies(QXmlStreamReader &xml, ModItem *mod);
    void parseModDependenciesByVersion(QXmlStreamReader &xml, ModItem *mod);
    QString parseDependencyItem(QXmlStreamReader &xml);
    void parseLoadBefore(QXmlStreamReader &xml, ModItem *mod);
    void parseLoadAfter(QXmlStreamReader &xml, ModItem *mod);
    void parseLoadBeforeByVersion(QXmlStreamReader &xml, ModItem *mod);
    void parseLoadAfterByVersion(QXmlStreamReader &xml, ModItem *mod);
    void parseForceLoadBefore(QXmlStreamReader &xml, ModItem *mod);
    void parseForceLoadAfter(QXmlStreamReader &xml, ModItem *mod);
    void parseIncompatibleWith(QXmlStreamReader &xml, ModItem *mod);
    void parseIncompatibleWithByVersion(QXmlStreamReader &xml, ModItem *mod);

    // 辅助方法：从注册表读取Steam路径（Windows）
    static QString getSteamPathFromRegistry();
};

#endif // WORKSHOPSCANNER_H
