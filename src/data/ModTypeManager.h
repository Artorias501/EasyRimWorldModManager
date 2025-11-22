#ifndef MODTYPEMANAGER_H
#define MODTYPEMANAGER_H

#include <QSet>
#include <QString>
#include <QStringList>

/**
 * @brief Mod类型管理器
 *
 * 管理所有可用的Mod类型，包括默认类型和用户自定义类型
 */
class ModTypeManager
{
public:
    ModTypeManager();

    // 获取所有可用的类型
    QStringList getAllTypes() const;

    // 添加自定义类型
    bool addCustomType(const QString &type);

    // 移除自定义类型
    bool removeCustomType(const QString &type);

    // 检查类型是否存在
    bool hasType(const QString &type) const;

    // 检查是否为默认类型
    bool isDefaultType(const QString &type) const;

    // 获取默认类型列表
    static QStringList getDefaultTypes();

    // 重置为默认类型
    void resetToDefaults();

    // 加载/保存自定义类型（可用于配置文件）
    void setCustomTypes(const QStringList &customTypes);
    QStringList getCustomTypes() const;

private:
    QSet<QString> m_customTypes; // 用户自定义类型

    static const QStringList s_defaultTypes; // 默认类型列表
};

#endif // MODTYPEMANAGER_H
