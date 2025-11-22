#include "OfficialDLCScanner.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>

OfficialDLCScanner::OfficialDLCScanner() {
}

OfficialDLCScanner::OfficialDLCScanner(const QString &dataPath)
    : m_dataPath(dataPath) {
}

void OfficialDLCScanner::setDataPath(const QString &path) {
    m_dataPath = path;
}

bool OfficialDLCScanner::scanAllDLCs() {
    clear();

    QDir dataDir(m_dataPath);
    if (!dataDir.exists()) {
        return false;
    }

    // 获取所有DLC目录
    QStringList dlcDirs = dataDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &dlcDirName: dlcDirs) {
        QString dlcDirPath = dataDir.absoluteFilePath(dlcDirName);
        ModItem *dlc = scanDLCDirectory(dlcDirPath);

        if (dlc && dlc->isValid()) {
            dlc->packageId = dlc->packageId.toLower();
            m_scannedDLCs.append(dlc);
            m_packageIdMap[dlc->packageId.toLower()] = dlc;
        } else if (dlc) {
            delete dlc; // 删除无效的DLC对象
        }
    }

    return true;
}

ModItem *OfficialDLCScanner::findDLCByPackageId(const QString &packageId) const {
    return m_packageIdMap.value(packageId, nullptr);
}

void OfficialDLCScanner::clear() {
    // 删除所有DLC对象
    qDeleteAll(m_scannedDLCs);
    m_scannedDLCs.clear();
    m_packageIdMap.clear();
}

QString OfficialDLCScanner::getDefaultDataPath(const QString &gameInstallPath) {
    if (gameInstallPath.isEmpty()) {
        return QString();
    }

    // 游戏安装路径下的Data文件夹
    return QDir(gameInstallPath).absoluteFilePath("Data");
}

ModItem *OfficialDLCScanner::scanDLCDirectory(const QString &dlcDirPath) {
    // 检查About.xml文件是否存在
    QString aboutXmlPath = QDir(dlcDirPath).absoluteFilePath("About/About.xml");

    if (!QFile::exists(aboutXmlPath)) {
        return nullptr;
    }

    ModItem *dlc = new ModItem();

    if (!parseAboutXml(dlc, aboutXmlPath)) {
        delete dlc;
        return nullptr;
    }

    dlc->sourcePath = dlcDirPath;

    // 判断是否为Core（Ludeon.RimWorld），Core不应标记为官方DLC
    if (dlc->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0) {
        dlc->isOfficialDLC = false;
        dlc->type = "核心";
        // 官方内容可能没有name字段，使用packageId
        if (dlc->name.isEmpty()) {
            dlc->name = "RimWorld Core";
        }
    } else {
        // 其他官方DLC
        dlc->isOfficialDLC = true;
        dlc->type = "DLC";
        // 官方DLC可能没有name字段，使用packageId作为fallback
        if (dlc->name.isEmpty()) {
            dlc->name = dlc->packageId;
        }
    }

    return dlc;
}

bool OfficialDLCScanner::parseAboutXml(ModItem *dlc, const QString &aboutXmlPath) {
    QFile file(aboutXmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QXmlStreamReader xml(&file);
    int depth = 0; // 跟踪 XML 深度

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            depth++;
            QString elementName = xml.name().toString();

            // 只处理 ModMetaData 直接子元素（深度为2）
            if (depth == 2) {
                if (elementName == "name") {
                    dlc->name = xml.readElementText();
                    depth--;
                } else if (elementName == "author" || elementName == "authors") {
                    dlc->author = xml.readElementText();
                    depth--;
                } else if (elementName == "description") {
                    dlc->description = xml.readElementText();
                    depth--;
                } else if (elementName == "packageId") {
                    // 只在顶层读取 packageId
                    QString packageId = xml.readElementText();
                    dlc->packageId = packageId;
                    dlc->identifier = packageId;
                    depth--;
                } else if (elementName == "steamAppId") {
                    QString steamId = xml.readElementText();
                    dlc->steamId = steamId;
                    depth--;
                } else if (elementName == "url") {
                    dlc->url = xml.readElementText();
                    depth--;
                } else if (elementName == "supportedVersions") {
                    parseSupportedVersions(xml, dlc);
                    depth--;
                } else if (elementName == "modDependencies") {
                    parseModDependencies(xml, dlc);
                    depth--;
                } else if (elementName == "modDependenciesByVersion") {
                    parseModDependenciesByVersion(xml, dlc);
                    depth--;
                } else if (elementName == "loadBefore") {
                    parseLoadBefore(xml, dlc);
                    depth--;
                } else if (elementName == "loadAfter") {
                    parseLoadAfter(xml, dlc);
                    depth--;
                } else if (elementName == "loadBeforeByVersion") {
                    parseLoadBeforeByVersion(xml, dlc);
                    depth--;
                } else if (elementName == "loadAfterByVersion") {
                    parseLoadAfterByVersion(xml, dlc);
                    depth--;
                } else if (elementName == "forceLoadBefore") {
                    parseForceLoadBefore(xml, dlc);
                    depth--;
                } else if (elementName == "forceLoadAfter") {
                    parseForceLoadAfter(xml, dlc);
                    depth--;
                } else if (elementName == "incompatibleWith") {
                    parseIncompatibleWith(xml, dlc);
                    depth--;
                } else if (elementName == "incompatibleWithByVersion") {
                    parseIncompatibleWithByVersion(xml, dlc);
                    depth--;
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            depth--;
        }
    }

    file.close();

    return !xml.hasError() && dlc->isValid();
}

void OfficialDLCScanner::parseSupportedVersions(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "supportedVersions")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addSupportedVersion(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseModDependencies(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "modDependencies")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            QString depPackageId = parseDependencyItem(xml);
            if (!depPackageId.isEmpty()) {
                mod->addDependency(depPackageId);
            }
        }
    }
}

void OfficialDLCScanner::parseModDependenciesByVersion(QXmlStreamReader &xml, ModItem *mod) {
    QSet<QString> allDependencies;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "modDependenciesByVersion")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v")) {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag)) {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li") {
                    QString depPackageId = parseDependencyItem(xml);
                    if (!depPackageId.isEmpty()) {
                        allDependencies.insert(depPackageId);
                    }
                }
            }
        }
    }

    for (const QString &dep: allDependencies) {
        mod->addDependency(dep);
    }
}

QString OfficialDLCScanner::parseDependencyItem(QXmlStreamReader &xml) {
    QString packageId;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "li")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "packageId") {
                packageId = xml.readElementText();
                break;
            }
        } else if (xml.tokenType() == QXmlStreamReader::Characters) {
            QString text = xml.text().toString().trimmed();
            if (!text.isEmpty()) {
                packageId = text;
            }
        }
    }

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "li")) {
        xml.readNext();
    }

    return packageId;
}

void OfficialDLCScanner::parseLoadBefore(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadBefore")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addLoadBefore(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseLoadAfter(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadAfter")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addLoadAfter(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseLoadBeforeByVersion(QXmlStreamReader &xml, ModItem *mod) {
    QSet<QString> allLoadBefore;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadBeforeByVersion")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v")) {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag)) {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li") {
                    allLoadBefore.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item: allLoadBefore) {
        mod->addLoadBefore(item);
    }
}

void OfficialDLCScanner::parseLoadAfterByVersion(QXmlStreamReader &xml, ModItem *mod) {
    QSet<QString> allLoadAfter;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadAfterByVersion")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v")) {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag)) {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li") {
                    allLoadAfter.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item: allLoadAfter) {
        mod->addLoadAfter(item);
    }
}

void OfficialDLCScanner::parseForceLoadBefore(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "forceLoadBefore")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addForceLoadBefore(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseForceLoadAfter(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "forceLoadAfter")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addForceLoadAfter(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseIncompatibleWith(QXmlStreamReader &xml, ModItem *mod) {
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "incompatibleWith")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li") {
            mod->addIncompatibleWith(xml.readElementText());
        }
    }
}

void OfficialDLCScanner::parseIncompatibleWithByVersion(QXmlStreamReader &xml, ModItem *mod) {
    QSet<QString> allIncompatible;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "incompatibleWithByVersion")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v")) {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag)) {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li") {
                    allIncompatible.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item: allIncompatible) {
        mod->addIncompatibleWith(item);
    }
}
