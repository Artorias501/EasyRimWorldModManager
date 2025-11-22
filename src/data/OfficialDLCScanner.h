#ifndef OFFICIALDLCSCANNER_H
#define OFFICIALDLCSCANNER_H

#include "ModItem.h"
#include <QList>
#include <QMap>
#include <QString>
#include <QXmlStreamReader>

/**
 * @brief 官方DLC扫描器
 *
 * 负责扫描RimWorld官方DLC目录，读取DLC的About.xml文件
 * 官方DLC路径: {游戏安装路径}/Data
 * 每个DLC目录包含: About\About.xml
 */
class OfficialDLCScanner
{
public:
    OfficialDLCScanner();

    explicit OfficialDLCScanner(const QString &dataPath);

    // 设置RimWorld Data路径
    void setDataPath(const QString &path);

    QString getDataPath() const { return m_dataPath; }

    // 扫描所有官方DLC
    bool scanAllDLCs();

    // 获取扫描到的DLC列表
    QList<ModItem *> getScannedDLCs() const { return m_scannedDLCs; }

    // 根据PackageId查找DLC
    ModItem *findDLCByPackageId(const QString &packageId) const;

    // 清除扫描结果
    void clear();

    // 获取默认Data路径（基于游戏安装路径）
    static QString getDefaultDataPath(const QString &gameInstallPath);

private:
    QString m_dataPath;                      // RimWorld Data路径
    QList<ModItem *> m_scannedDLCs;          // 扫描到的DLC列表
    QMap<QString, ModItem *> m_packageIdMap; // PackageId到DLC的映射

    // 扫描单个DLC目录
    ModItem *scanDLCDirectory(const QString &dlcDirPath);

    // 解析About.xml文件
    bool parseAboutXml(ModItem *dlc, const QString &aboutXmlPath);

    // 辅助解析方法
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
};

#endif // OFFICIALDLCSCANNER_H
