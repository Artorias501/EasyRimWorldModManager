#ifndef USERDATAMANAGER_H
#define USERDATAMANAGER_H

#include <QMap>
#include <QString>
#include <QStringList>

/**
 * @brief 用户数据管理器
 *
 * 管理用户的持久化数据，包括：
 * 1. Mod类型和备注的映射
 * 2. 保存的Mod加载列表
 *
 * 数据存储在程序同目录下的UserData文件夹中
 */
class UserDataManager
{
public:
    UserDataManager();

    // ==================== Mod类型和备注管理 ====================

    // 获取Mod的类型
    QString getModType(const QString &packageId) const;

    // 设置Mod的类型
    void setModType(const QString &packageId, const QString &type);

    // 移除Mod的类型
    void removeModType(const QString &packageId);

    // 获取Mod的备注
    QString getModRemark(const QString &packageId) const;

    // 设置Mod的备注
    void setModRemark(const QString &packageId, const QString &remark);

    // 移除Mod的备注
    void removeModRemark(const QString &packageId);

    // 获取所有自定义类型
    QStringList getAllCustomTypes() const;

    // 获取所有可用类型（默认类型 + 自定义类型）
    QStringList getAllTypes() const;

    // 获取默认类型列表
    static QStringList getDefaultTypes();

    // 添加自定义类型
    bool addCustomType(const QString &type);

    // 移除自定义类型
    bool removeCustomType(const QString &type);

    // 检查类型是否存在（包括默认类型和自定义类型）
    bool hasType(const QString &type) const;

    // 检查是否为默认类型
    bool isDefaultType(const QString &type) const;

    // 检查自定义类型是否存在
    bool hasCustomType(const QString &type) const;

    // ==================== 类型优先级管理 ====================

    // 获取类型优先级列表（从高到低）
    QStringList getTypePriority() const;

    // 设置类型优先级列表
    void setTypePriority(const QStringList &priority);

    // 加载类型优先级
    bool loadTypePriority();

    // 保存类型优先级
    bool saveTypePriority();

    // ==================== 数据持久化 ====================

    // 加载Mod数据（类型和备注）
    bool loadModData();

    // 保存Mod数据（类型和备注）
    bool saveModData();

    // 加载自定义类型列表
    bool loadCustomTypes();

    // 保存自定义类型列表
    bool saveCustomTypes();

    // 加载所有用户数据
    bool loadAll();

    // 保存所有用户数据
    bool saveAll();

    // ==================== Mod列表管理 ====================

    // 保存Mod列表到指定路径
    bool saveModListToPath(const QString &filePath,
                           const QStringList &activeMods,
                           const QStringList &knownExpansions,
                           const QString &version,
                           const QMap<QString, QString> &otherData = QMap<QString, QString>());

    // 列出所有保存的Mod列表
    QStringList listSavedModLists() const;

    // 删除保存的Mod列表
    bool deleteSavedModList(const QString &fileName);

    // ==================== 路径管理 ====================

    // 获取用户数据根目录
    static QString getUserDataPath();

    // 获取Mod数据目录
    static QString getModDataPath();

    // 获取Mod列表目录
    static QString getModListPath();

    // 初始化目录结构
    static bool initializeDirectories();

private:
    QMap<QString, QString> m_modTypes;   // PackageId -> Type
    QMap<QString, QString> m_modRemarks; // PackageId -> Remark
    QStringList m_customTypes;           // 自定义类型列表
    QStringList m_typePriority;          // 类型优先级列表（从高到低）

    // 默认类型列表
    static const QStringList s_defaultTypes;

    // 文件名常量
    static const QString MOD_DATA_FILE;
    static const QString CUSTOM_TYPES_FILE;
    static const QString TYPE_PRIORITY_FILE;
};

#endif // USERDATAMANAGER_H
