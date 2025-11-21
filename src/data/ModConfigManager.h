#ifndef MODCONFIGMANAGER_H
#define MODCONFIGMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>

class ModItem;

/**
 * @brief ModsConfig.xml 配置文件管理器
 *
 * 负责读取和写入 RimWorld 的 ModsConfig.xml 文件
 * 该文件位于 C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
 */
class ModConfigManager
{
public:
    ModConfigManager();
    explicit ModConfigManager(const QString &configPath);

    // 读取配置
    bool loadConfig();
    bool loadConfig(const QString &configPath);

    // 保存配置
    bool saveConfig();
    bool saveConfig(const QString &configPath);

    // 获取当前激活的Mod列表（按加载顺序）
    QStringList getActiveMods() const { return m_activeMods; }

    // 获取已知的扩展包列表
    QStringList getKnownExpansions() const { return m_knownExpansions; }

    // 获取游戏版本
    QString getVersion() const { return m_version; }

    // 设置激活的Mod列表
    void setActiveMods(const QStringList &mods);

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

    // 自动检测配置文件路径
    static QString getDefaultConfigPath();

private:
    QString m_configPath;          // 配置文件路径
    QString m_version;             // 游戏版本
    QStringList m_activeMods;      // 激活的Mod列表（按加载顺序）
    QStringList m_knownExpansions; // 已知的扩展包

    // 辅助方法
    bool parseXml(const QString &xmlContent);
    QString generateXml() const;
};

#endif // MODCONFIGMANAGER_H
