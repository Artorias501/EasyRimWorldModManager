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
const QString UserDataManager::TYPES_FILE = "types.json";
const QString UserDataManager::TYPE_PRIORITY_FILE = "type_priority.json";

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

QStringList UserDataManager::getAllTypes() const
{
    return m_types;
}

bool UserDataManager::addType(const QString &type)
{
    if (type.isEmpty())
    {
        return false; // 不能添加空类型
    }

    if (m_types.contains(type))
    {
        return false; // 已存在
    }

    m_types.append(type);
    return true;
}

bool UserDataManager::removeType(const QString &type)
{
    return m_types.removeOne(type);
}

bool UserDataManager::hasType(const QString &type) const
{
    return m_types.contains(type);
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

bool UserDataManager::loadTypes()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(TYPES_FILE);
    QFile file(filePath);

    if (!file.exists())
    {
        qDebug() << "Mod类型文件不存在，使用默认类型（核心、DLC）";
        // 首次加载，添加默认的两个类型
        m_types.clear();
        m_types.append("核心");
        m_types.append("DLC");
        return true;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "无法打开Mod类型文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        qWarning() << "Mod类型文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    m_types.clear();

    if (root.contains("types") && root["types"].isArray())
    {
        QJsonArray types = root["types"].toArray();
        for (const QJsonValue &value : types)
        {
            QString type = value.toString();
            if (!type.isEmpty())
            {
                m_types.append(type);
            }
        }
    }

    qDebug() << "成功加载" << m_types.size() << "个Mod类型";
    return true;
}

bool UserDataManager::saveTypes()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(TYPES_FILE);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "无法创建Mod类型文件:" << filePath;
        return false;
    }

    QJsonObject root;
    QJsonArray types;

    for (const QString &type : m_types)
    {
        types.append(type);
    }

    root["types"] = types;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "成功保存Mod类型到:" << filePath;
    return true;
}

// ==================== 类型优先级管理 ====================

QStringList UserDataManager::getTypePriority() const
{
    return m_typePriority;
}

void UserDataManager::setTypePriority(const QStringList &priority)
{
    m_typePriority = priority;
}

bool UserDataManager::loadTypePriority()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(TYPE_PRIORITY_FILE);

    QFile file(filePath);
    if (!file.exists())
    {
        qDebug() << "类型优先级文件不存在，使用默认顺序";
        m_typePriority.clear();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法打开类型优先级文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        qWarning() << "类型优先级文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray priorityArray = root.value("priority").toArray();

    m_typePriority.clear();
    for (const QJsonValue &value : priorityArray)
    {
        m_typePriority.append(value.toString());
    }

    qDebug() << "成功加载类型优先级:" << m_typePriority.size() << "个类型";
    return true;
}

bool UserDataManager::saveTypePriority()
{
    QString filePath = QDir(getModDataPath()).absoluteFilePath(TYPE_PRIORITY_FILE);

    QJsonObject root;
    QJsonArray priorityArray;

    for (const QString &type : m_typePriority)
    {
        priorityArray.append(type);
    }

    root["priority"] = priorityArray;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "无法写入类型优先级文件:" << filePath;
        return false;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "成功保存类型优先级到:" << filePath;
    return true;
}

bool UserDataManager::loadAll()
{
    bool success = true;
    success &= loadModData();
    success &= loadTypes();
    success &= loadTypePriority();
    return success;
}

bool UserDataManager::saveAll()
{
    bool success = true;
    success &= saveModData();
    success &= saveTypes();
    success &= saveTypePriority();
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
