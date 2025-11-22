#include "UserDataManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// 定义文件名常量
const QString UserDataManager::MOD_DATA_FILE = "mod_data.json";
const QString UserDataManager::CUSTOM_TYPES_FILE = "custom_types.json";

// 定义默认类型列表
const QStringList UserDataManager::s_defaultTypes = {
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

UserDataManager::UserDataManager()
{
    // 初始化目录结构
    initializeDirectories();
}

// ==================== Mod类型和备注管理 ====================

QString UserDataManager::getModType(const QString &packageId) const
{
    return m_modTypes.value(packageId, QString());
}

void UserDataManager::setModType(const QString &packageId, const QString &type)
{
    if (!packageId.isEmpty())
    {
        m_modTypes[packageId] = type;
    }
}

void UserDataManager::removeModType(const QString &packageId)
{
    m_modTypes.remove(packageId);
}

QString UserDataManager::getModRemark(const QString &packageId) const
{
    return m_modRemarks.value(packageId, QString());
}

void UserDataManager::setModRemark(const QString &packageId, const QString &remark)
{
    if (!packageId.isEmpty())
    {
        m_modRemarks[packageId] = remark;
    }
}

void UserDataManager::removeModRemark(const QString &packageId)
{
    m_modRemarks.remove(packageId);
}

QStringList UserDataManager::getAllCustomTypes() const
{
    return m_customTypes;
}

QStringList UserDataManager::getAllTypes() const
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

QStringList UserDataManager::getDefaultTypes()
{
    return s_defaultTypes;
}

bool UserDataManager::addCustomType(const QString &type)
{
    if (type.isEmpty() || s_defaultTypes.contains(type))
    {
        return false; // 不能添加空类型或与默认类型重复的类型
    }

    if (m_customTypes.contains(type))
    {
        return false; // 已存在
    }

    m_customTypes.append(type);
    return true;
}

bool UserDataManager::removeCustomType(const QString &type)
{
    if (s_defaultTypes.contains(type))
    {
        return false; // 不能删除默认类型
    }

    return m_customTypes.removeOne(type);
}

bool UserDataManager::hasType(const QString &type) const
{
    return s_defaultTypes.contains(type) || m_customTypes.contains(type);
}

bool UserDataManager::isDefaultType(const QString &type) const
{
    return s_defaultTypes.contains(type);
}

bool UserDataManager::hasCustomType(const QString &type) const
{
    return m_customTypes.contains(type);
}

// ==================== 数据持久化 ====================

bool UserDataManager::loadModData()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(MOD_DATA_FILE);
    QFile file(filePath);

    if (!file.exists())
    {
        qDebug() << "Mod数据文件不存在，使用空数据";
        return true; // 文件不存在不算错误
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "无法打开Mod数据文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        qWarning() << "Mod数据文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();

    // 加载类型映射
    m_modTypes.clear();
    if (root.contains("types") && root["types"].isObject())
    {
        QJsonObject types = root["types"].toObject();
        for (auto it = types.begin(); it != types.end(); ++it)
        {
            m_modTypes[it.key()] = it.value().toString();
        }
    }

    // 加载备注映射
    m_modRemarks.clear();
    if (root.contains("remarks") && root["remarks"].isObject())
    {
        QJsonObject remarks = root["remarks"].toObject();
        for (auto it = remarks.begin(); it != remarks.end(); ++it)
        {
            m_modRemarks[it.key()] = it.value().toString();
        }
    }

    qDebug() << "成功加载Mod数据:" << m_modTypes.size() << "个类型," << m_modRemarks.size() << "个备注";
    return true;
}

bool UserDataManager::saveModData()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(MOD_DATA_FILE);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "无法创建Mod数据文件:" << filePath;
        return false;
    }

    QJsonObject root;

    // 保存类型映射
    QJsonObject types;
    for (auto it = m_modTypes.begin(); it != m_modTypes.end(); ++it)
    {
        types[it.key()] = it.value();
    }
    root["types"] = types;

    // 保存备注映射
    QJsonObject remarks;
    for (auto it = m_modRemarks.begin(); it != m_modRemarks.end(); ++it)
    {
        remarks[it.key()] = it.value();
    }
    root["remarks"] = remarks;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "成功保存Mod数据到:" << filePath;
    return true;
}

bool UserDataManager::loadCustomTypes()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(CUSTOM_TYPES_FILE);
    QFile file(filePath);

    if (!file.exists())
    {
        qDebug() << "自定义类型文件不存在，使用空列表";
        return true;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "无法打开自定义类型文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        qWarning() << "自定义类型文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    m_customTypes.clear();

    if (root.contains("customTypes") && root["customTypes"].isArray())
    {
        QJsonArray types = root["customTypes"].toArray();
        for (const QJsonValue &value : types)
        {
            QString type = value.toString();
            if (!type.isEmpty())
            {
                m_customTypes.append(type);
            }
        }
    }

    qDebug() << "成功加载" << m_customTypes.size() << "个自定义类型";
    return true;
}

bool UserDataManager::saveCustomTypes()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(CUSTOM_TYPES_FILE);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "无法创建自定义类型文件:" << filePath;
        return false;
    }

    QJsonObject root;
    QJsonArray types;

    for (const QString &type : m_customTypes)
    {
        types.append(type);
    }

    root["customTypes"] = types;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "成功保存自定义类型到:" << filePath;
    return true;
}

bool UserDataManager::loadAll()
{
    bool success = true;
    success &= loadModData();
    success &= loadCustomTypes();
    return success;
}

bool UserDataManager::saveAll()
{
    bool success = true;
    success &= saveModData();
    success &= saveCustomTypes();
    return success;
}

// ==================== Mod列表管理 ====================

bool UserDataManager::saveModListToPath(const QString &filePath,
                                        const QStringList &activeMods,
                                        const QStringList &knownExpansions,
                                        const QString &version,
                                        const QMap<QString, QString> &otherData)
{
    QFile file(filePath);

    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "无法创建Mod列表文件:" << filePath;
        return false;
    }

    // 写入XML
    QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml += "<ModsConfigData>\n";

    // 写入version
    if (!version.isEmpty())
    {
        xml += QString("  <version>%1</version>\n").arg(version);
    }

    // 写入activeMods
    xml += "  <activeMods>\n";
    for (const QString &modId : activeMods)
    {
        xml += QString("    <li>%1</li>\n").arg(modId);
    }
    xml += "  </activeMods>\n";

    // 写入knownExpansions
    xml += "  <knownExpansions>\n";
    for (const QString &expansionId : knownExpansions)
    {
        xml += QString("    <li>%1</li>\n").arg(expansionId);
    }
    xml += "  </knownExpansions>\n";

    // 写入其他数据（如果有）
    for (auto it = otherData.begin(); it != otherData.end(); ++it)
    {
        xml += QString("  <%1>%2</%1>\n").arg(it.key(), it.value());
    }

    xml += "</ModsConfigData>\n";

    file.write(xml.toUtf8());
    file.close();

    qDebug() << "成功保存Mod列表到:" << filePath;
    return true;
}

QStringList UserDataManager::listSavedModLists() const
{
    QString modListPath = getModListPath();
    QDir dir(modListPath);

    if (!dir.exists())
    {
        return QStringList();
    }

    // 列出所有XML文件
    QStringList filters;
    filters << "*.xml";
    return dir.entryList(filters, QDir::Files, QDir::Name);
}

bool UserDataManager::deleteSavedModList(const QString &fileName)
{
    QString filePath = QDir(getModListPath()).absoluteFilePath(fileName);
    QFile file(filePath);

    if (!file.exists())
    {
        qWarning() << "文件不存在:" << filePath;
        return false;
    }

    bool success = file.remove();
    if (success)
    {
        qDebug() << "成功删除Mod列表:" << filePath;
    }
    else
    {
        qWarning() << "无法删除Mod列表:" << filePath;
    }

    return success;
}

// ==================== 路径管理 ====================

QString UserDataManager::getUserDataPath()
{
    // 获取程序所在目录
    QString appDir = QCoreApplication::applicationDirPath();
    return QDir(appDir).absoluteFilePath("UserData");
}

QString UserDataManager::getModDataPath()
{
    return QDir(getUserDataPath()).absoluteFilePath("Mod");
}

QString UserDataManager::getModListPath()
{
    return QDir(getUserDataPath()).absoluteFilePath("ModList");
}

bool UserDataManager::initializeDirectories()
{
    QDir dir;

    // 创建UserData目录
    QString userDataPath = getUserDataPath();
    if (!dir.exists(userDataPath))
    {
        if (!dir.mkpath(userDataPath))
        {
            qWarning() << "无法创建UserData目录:" << userDataPath;
            return false;
        }
        qDebug() << "创建UserData目录:" << userDataPath;
    }

    // 创建Mod目录
    QString modDataPath = getModDataPath();
    if (!dir.exists(modDataPath))
    {
        if (!dir.mkpath(modDataPath))
        {
            qWarning() << "无法创建Mod数据目录:" << modDataPath;
            return false;
        }
        qDebug() << "创建Mod数据目录:" << modDataPath;
    }

    // 创建ModList目录
    QString modListPath = getModListPath();
    if (!dir.exists(modListPath))
    {
        if (!dir.mkpath(modListPath))
        {
            qWarning() << "无法创建ModList目录:" << modListPath;
            return false;
        }
        qDebug() << "创建ModList目录:" << modListPath;
    }

    return true;
}
