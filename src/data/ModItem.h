#ifndef MODITEM_H
#define MODITEM_H

#include <QString>
#include <QStringList>

/**
 * @brief Mod项数据结构
 *
 * 存储单个Mod的所有信息，包括标识符、名称、描述等
 */
struct ModItem
{
    QString identifier;            // 唯一标识符（PackageId）
    QString name;                  // Mod名称
    QString description;           // Mod描述
    QString author;                // 作者名
    QString url;                   // Mod链接
    QString packageId;             // 包ID（与identifier相同，保留以便扩展）
    QString steamId;               // Steam工坊ID（如适用）
    QStringList supportedVersions; // 支持的游戏版本
    QStringList dependencies;      // 依赖的Mod列表
    QStringList loadBefore;        // 需要在这些Mod之前加载
    QStringList loadAfter;         // 需要在这些Mod之后加载
    QStringList forceLoadBefore;   // 强制在这些Mod之前加载（硬性约束）
    QStringList forceLoadAfter;    // 强制在这些Mod之后加载（硬性约束）
    QStringList incompatibleWith;  // 不兼容的Mod列表

    QString remark;             // 备注
    QString type;               // Mod类型（如：核心、DLC、前置框架等）
    bool isOfficialDLC = false; // 是否为官方DLC（默认为false）
    QString sourcePath;         // Mod来源路径（用于区分工坊mod和官方DLC）

    // 辅助方法
    void addDependency(const QString &dependency);

    void addLoadBefore(const QString &modId);

    void addLoadAfter(const QString &modId);

    void addForceLoadBefore(const QString &modId);

    void addForceLoadAfter(const QString &modId);

    void addIncompatibleWith(const QString &modId);

    void addSupportedVersion(const QString &version);

    bool isValid() const;
};

#endif // MODITEM_H
