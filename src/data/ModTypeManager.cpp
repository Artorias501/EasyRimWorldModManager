#include "ModTypeManager.h"

// 定义默认类型列表
const QStringList ModTypeManager::s_defaultTypes = {
    "核心",
    "DLC",
    "前置框架",
    "逻辑mod",
    "功能性mod",
    "种族mod",
    "种族扩展mod",
    "单一功能mod",
    "汉化",
    "优化"};

ModTypeManager::ModTypeManager()
{
}

QStringList ModTypeManager::getAllTypes() const
{
    QStringList allTypes = s_defaultTypes;

    // 添加自定义类型
    for (const QString &type : m_customTypes)
    {
        if (!allTypes.contains(type))
        {
            allTypes.append(type);
        }
    }

    return allTypes;
}

bool ModTypeManager::addCustomType(const QString &type)
{
    if (type.isEmpty() || s_defaultTypes.contains(type))
    {
        return false; // 不能添加空类型或与默认类型重复的类型
    }

    if (m_customTypes.contains(type))
    {
        return false; // 已存在
    }

    m_customTypes.insert(type);
    return true;
}

bool ModTypeManager::removeCustomType(const QString &type)
{
    if (s_defaultTypes.contains(type))
    {
        return false; // 不能删除默认类型
    }

    return m_customTypes.remove(type) > 0;
}

bool ModTypeManager::hasType(const QString &type) const
{
    return s_defaultTypes.contains(type) || m_customTypes.contains(type);
}

bool ModTypeManager::isDefaultType(const QString &type) const
{
    return s_defaultTypes.contains(type);
}

QStringList ModTypeManager::getDefaultTypes()
{
    return s_defaultTypes;
}

void ModTypeManager::resetToDefaults()
{
    m_customTypes.clear();
}

void ModTypeManager::setCustomTypes(const QStringList &customTypes)
{
    m_customTypes.clear();
    for (const QString &type : customTypes)
    {
        if (!type.isEmpty() && !s_defaultTypes.contains(type))
        {
            m_customTypes.insert(type);
        }
    }
}

QStringList ModTypeManager::getCustomTypes() const
{
    return m_customTypes.values();
}
