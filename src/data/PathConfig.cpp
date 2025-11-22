#include "PathConfig.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

PathConfig::PathConfig() {
}

bool PathConfig::load() {
    QString filePath = getConfigFilePath();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开路径配置文件:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "路径配置文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    m_gameInstallPath = root.value("gameInstallPath").toString();
    m_steamPath = root.value("steamPath").toString();
    m_userSavePath = root.value("userSavePath").toString();

    qDebug() << "路径配置已加载";
    qDebug() << "  游戏路径:" << m_gameInstallPath;
    qDebug() << "  Steam路径:" << m_steamPath;
    qDebug() << "  存档路径:" << m_userSavePath;

    return true;
}

bool PathConfig::save() const {
    QString filePath = getConfigFilePath();

    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "无法创建配置目录:" << dir.path();
            return false;
        }
    }

    QJsonObject root;
    root["gameInstallPath"] = m_gameInstallPath;
    root["steamPath"] = m_steamPath;
    root["userSavePath"] = m_userSavePath;

    QJsonDocument doc(root);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法写入路径配置文件:" << filePath;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "路径配置已保存到:" << filePath;
    return true;
}

bool PathConfig::configExists() {
    QString filePath = getConfigFilePath();
    return QFile::exists(filePath);
}

bool PathConfig::isValid() const {
    return !m_gameInstallPath.isEmpty() &&
           !m_steamPath.isEmpty() &&
           !m_userSavePath.isEmpty();
}

QString PathConfig::getDefaultUserSavePath() {
    // RimWorld 默认存档路径
    // Windows: C:/Users/{username}/AppData/LocalLow/Ludeon Studios/RimWorld by Ludeon Studios
    QString localLowPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    localLowPath = localLowPath.replace("/Local", "/LocalLow");

    QString savePath = QDir(localLowPath).absoluteFilePath("Ludeon Studios/RimWorld by Ludeon Studios");

    return savePath;
}

QString PathConfig::getConfigFilePath() {
    // 配置文件路径：程序目录/UserData/path.json
    QString appDir = QDir::currentPath();
    return QDir(appDir).absoluteFilePath("UserData/path.json");
}
