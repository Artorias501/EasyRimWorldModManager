#ifndef MODCONFIGMANAGER_H
#define MODCONFIGMANAGER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

struct ModItem; // 前向声明

/**
 * @brief ModsConfig.xml 配置文件管理器
 *
 * 负责读取和写入 RimWorld 的 ModsConfig.xml 文件
 * 该文件位于 C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
 *
 * 注意：会保留原文件中除 activeMods 和 knownExpansions 外的所有字段
 */
class ModConfigManager
{
public:
    ModConfigManager();

    explicit ModConfigManager(const QString &configPath);

    // 读取配置
    bool loadConfig();

    bool loadConfig(const QString &configPath);

    // 创建空白加载列表（从默认配置文件读取版本和其他字段，但activeMods和knownExpansions为空）
    bool loadConfigWithEmptyMods();

    bool loadConfigWithEmptyMods(const QString &configPath);

    // 保存配置
    bool saveConfig();

    bool saveConfig(const QString &configPath);

    // 获取当前激活的Mod列表（按加载顺序）
    // 注意：activeMods包含所有激活的mod（核心、DLC、创意工坊mod）
    QStringList getActiveMods() const { return m_activeMods; }

    // 获取已知的扩展包列表
    // 注意：knownExpansions仅包含官方DLC的packageId，不包含核心和创意工坊mod
    QStringList getKnownExpansions() const { return m_knownExpansions; }

    // 获取游戏版本
    QString getVersion() const { return m_version; }

    // 设置游戏版本（通常从原文件读取，不需要修改）
    void setVersion(const QString &version) { m_version = version; }

    // 设置已知扩展包列表（通常从原文件读取，不需要修改）
    void setKnownExpansions(const QStringList &expansions) { m_knownExpansions = expansions; }

    // 设置激活的Mod列表
    void setActiveMods(const QStringList &mods);

    // 从ModItem列表设置配置（自动区分activeMods和knownExpansions）
    void setActiveModsFromList(const QList<ModItem *> &modList);

    // 添加/移除 Mod
    void addMod(const QString &modId);

    void removeMod(const QString &modId);

    void insertMod(int index, const QString &modId);

    // Mod排序操作
    bool moveModUp(const QString &modId);

    bool moveModDown(const QString &modId);

    bool moveModToPosition(const QString &modId, int newPosition);

    // 检查Mod是否激活
    bool isModActive(const QString &modId) const;

    // 获取Mod在加载顺序中的位置
    int getModPosition(const QString &modId) const;

    // 获取配置文件路径
    QString getConfigPath() const { return m_configPath; }

    // 获取其他字段数据（用于完整保存文件）
    QMap<QString, QString> getOtherFields() const { return m_otherFields; }

    // 自动检测配置文件路径
    static QString getDefaultConfigPath();

private:
    QString m_configPath;                 // 配置文件路径
    QString m_version;                    // 游戏版本
    QStringList m_activeMods;             // 激活的Mod列表（按加载顺序）
    QStringList m_knownExpansions;        // 已知的扩展包
    QMap<QString, QString> m_otherFields; // 其他字段（保持原样）

    // 辅助方法
    bool parseXml(const QString &xmlContent);

    QString generateXml() const;
};

#endif // MODCONFIGMANAGER_H
