#include "WorkshopScanner.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QXmlStreamReader>

WorkshopScanner::WorkshopScanner()
    : m_workshopPath(getDefaultWorkshopPath()) {
}

WorkshopScanner::WorkshopScanner(const QString &workshopPath)
    : m_workshopPath(workshopPath) {
}

void WorkshopScanner::setWorkshopPath(const QString &path) {
    m_workshopPath = path;
}

bool WorkshopScanner::scanAllMods() {
    clear();

    QDir workshopDir(m_workshopPath);
    if (!workshopDir.exists()) {
        return false;
    }

    // 获取所有Mod目录（每个目录名是Steam WorkshopId）
    QStringList modDirs = workshopDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &workshopId: modDirs) {
        QString modDirPath = workshopDir.absoluteFilePath(workshopId);
        ModItem *mod = scanModDirectory(modDirPath, workshopId);

        if (mod && mod->isValid()) {
            mod->packageId = mod->packageId.toLower();
            m_scannedMods.append(mod);
            m_packageIdMap[mod->packageId] = mod;
            m_workshopIdMap[workshopId] = mod;
        } else if (mod) {
            delete mod; // 删除无效的Mod对象
        }
    }

    return true;
}

ModItem *WorkshopScanner::findModByPackageId(const QString &packageId) const {
    return m_packageIdMap.value(packageId, nullptr);
}

ModItem *WorkshopScanner::findModByWorkshopId(const QString &workshopId) const {
    return m_workshopIdMap.value(workshopId, nullptr);
}

void WorkshopScanner::clear() {
    // 删除所有Mod对象
    qDeleteAll(m_scannedMods);
    m_scannedMods.clear();
    m_packageIdMap.clear();
    m_workshopIdMap.clear();
}

QString WorkshopScanner::detectSteamPath() {
    // 尝试从注册表读取
    QString steamPath = getSteamPathFromRegistry();
    if (!steamPath.isEmpty() && QDir(steamPath).exists()) {
        return steamPath;
    }

    // 尝试常见的默认路径
    QStringList commonPaths = {
        "C:/Program Files (x86)/Steam",
        "C:/Program Files/Steam",
        "D:/Steam",
        "E:/Steam",
        "F:/Steam"
    };

    for (const QString &path: commonPaths) {
        if (QDir(path).exists()) {
            return path;
        }
    }

    return QString();
}

QString WorkshopScanner::getDefaultWorkshopPath() {
    QString steamPath = detectSteamPath();
    if (steamPath.isEmpty()) {
        return QString();
    }

    // RimWorld的Steam AppId是294100
    return QDir(steamPath).absoluteFilePath("steamapps/workshop/content/294100");
}

ModItem *WorkshopScanner::scanModDirectory(const QString &modDirPath, const QString &workshopId) {
    // 检查About.xml文件是否存在
    QString aboutXmlPath = QDir(modDirPath).absoluteFilePath("About/About.xml");

    if (!QFile::exists(aboutXmlPath)) {
        return nullptr;
    }

    ModItem *mod = new ModItem();

    if (!parseAboutXml(mod, aboutXmlPath)) {
        delete mod;
        return nullptr;
    }

    // 标记为非官方DLC（这是来自Steam创意工坊的mod）
    mod->isOfficialDLC = false;
    mod->sourcePath = modDirPath;

    return mod;
}

bool WorkshopScanner::parseAboutXml(ModItem *mod, const QString &aboutXmlPath) {
    QFile file(aboutXmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = xml.name().toString();

            if (elementName == "name") {
                mod->name = xml.readElementText();
            } else if (elementName == "author") {
                mod->author = xml.readElementText();
            } else if (elementName == "description") {
                mod->description = xml.readElementText();
            } else if (elementName == "packageId") {
                QString packageId = xml.readElementText();
                mod->packageId = packageId;
                mod->identifier = packageId;
            } else if (elementName == "url") {
                mod->url = xml.readElementText();
            } else if (elementName == "supportedVersions") {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "supportedVersions")) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li") {
                        mod->addSupportedVersion(xml.readElementText());
                    }
                }
            } else if (elementName == "modDependencies") {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "modDependencies")) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li") {
                        // 在<li>中查找<packageId>
                        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                                 xml.name() == "li")) {
                            xml.readNext();
                            if (xml.tokenType() == QXmlStreamReader::StartElement &&
                                xml.name() == "packageId") {
                                mod->addDependency(xml.readElementText());
                            }
                        }
                    }
                }
            } else if (elementName == "loadBefore") {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "loadBefore")) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li") {
                        mod->addLoadBefore(xml.readElementText());
                    }
                }
            } else if (elementName == "loadAfter") {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "loadAfter")) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li") {
                        mod->addLoadAfter(xml.readElementText());
                    }
                }
            } else if (elementName == "incompatibleWith") {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "incompatibleWith")) {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li") {
                        mod->addIncompatibleWith(xml.readElementText());
                    }
                }
            }
        }
    }

    file.close();

    return !xml.hasError() && mod->isValid();
}

QString WorkshopScanner::getSteamPathFromRegistry() {
#ifdef Q_OS_WIN
    // 从Windows注册表读取Steam安装路径
    QSettings steamReg("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
    QString steamPath = steamReg.value("SteamPath").toString();

    if (!steamPath.isEmpty()) {
        // 将路径中的 / 替换为 \ （如果需要）
        return QDir::fromNativeSeparators(steamPath);
    }

    // 尝试另一个位置
    QSettings steamReg2("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
    steamPath = steamReg2.value("InstallPath").toString();

    if (!steamPath.isEmpty()) {
        return QDir::fromNativeSeparators(steamPath);
    }
#endif

    return QString();
}
