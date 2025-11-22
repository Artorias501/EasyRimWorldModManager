#include "WorkshopScanner.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QXmlStreamReader>

WorkshopScanner::WorkshopScanner()
    : m_workshopPath(getDefaultWorkshopPath())
{
}

WorkshopScanner::WorkshopScanner(const QString &workshopPath)
    : m_workshopPath(workshopPath)
{
}

void WorkshopScanner::setWorkshopPath(const QString &path)
{
    m_workshopPath = path;
}

bool WorkshopScanner::scanAllMods()
{
    clear();

    QDir workshopDir(m_workshopPath);
    if (!workshopDir.exists())
    {
        return false;
    }

    // 获取所有Mod目录（每个目录名是Steam WorkshopId）
    QStringList modDirs = workshopDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &workshopId : modDirs)
    {
        QString modDirPath = workshopDir.absoluteFilePath(workshopId);
        ModItem *mod = scanModDirectory(modDirPath, workshopId);

        if (mod && mod->isValid())
        {
            mod->packageId = mod->packageId.toLower();
            mod->steamId = workshopId;
            m_scannedMods.append(mod);
            m_packageIdMap[mod->packageId] = mod;
            m_workshopIdMap[workshopId] = mod;
        }
        else if (mod)
        {
            delete mod; // 删除无效的Mod对象
        }
    }

    return true;
}

ModItem *WorkshopScanner::findModByPackageId(const QString &packageId) const
{
    return m_packageIdMap.value(packageId, nullptr);
}

ModItem *WorkshopScanner::findModByWorkshopId(const QString &workshopId) const
{
    return m_workshopIdMap.value(workshopId, nullptr);
}

void WorkshopScanner::clear()
{
    // 删除所有Mod对象
    qDeleteAll(m_scannedMods);
    m_scannedMods.clear();
    m_packageIdMap.clear();
    m_workshopIdMap.clear();
}

QString WorkshopScanner::detectSteamPath()
{
    // 尝试从注册表读取
    QString steamPath = getSteamPathFromRegistry();
    if (!steamPath.isEmpty() && QDir(steamPath).exists())
    {
        return steamPath;
    }

    // 尝试常见的默认路径
    QStringList commonPaths = {
        "C:/Program Files (x86)/Steam",
        "C:/Program Files/Steam",
        "D:/Steam",
        "E:/Steam",
        "F:/Steam"};

    for (const QString &path : commonPaths)
    {
        if (QDir(path).exists())
        {
            return path;
        }
    }

    return QString();
}

QString WorkshopScanner::getDefaultWorkshopPath()
{
    QString steamPath = detectSteamPath();
    if (steamPath.isEmpty())
    {
        return QString();
    }

    // RimWorld的Steam AppId是294100
    return QDir(steamPath).absoluteFilePath("steamapps/workshop/content/294100");
}

ModItem *WorkshopScanner::scanModDirectory(const QString &modDirPath, const QString &workshopId)
{
    // 检查About.xml文件是否存在
    QString aboutXmlPath = QDir(modDirPath).absoluteFilePath("About/About.xml");

    if (!QFile::exists(aboutXmlPath))
    {
        return nullptr;
    }

    ModItem *mod = new ModItem();

    if (!parseAboutXml(mod, aboutXmlPath))
    {
        delete mod;
        return nullptr;
    }

    // 标记为非官方DLC（这是来自Steam创意工坊的mod）
    mod->isOfficialDLC = false;
    mod->sourcePath = modDirPath;

    return mod;
}

bool WorkshopScanner::parseAboutXml(ModItem *mod, const QString &aboutXmlPath)
{
    QFile file(aboutXmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QXmlStreamReader xml(&file);
    int depth = 0; // 跟踪 XML 深度，确保只读取顶层元素

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            depth++;
            QString elementName = xml.name().toString();

            // 只处理 ModMetaData 直接子元素（深度为2）
            if (depth == 2)
            {
                if (elementName == "name")
                {
                    mod->name = xml.readElementText();
                    depth--; // readElementText 会读取到结束标签
                }
                else if (elementName == "author" || elementName == "authors")
                {
                    mod->author = xml.readElementText();
                    depth--;
                }
                else if (elementName == "description")
                {
                    mod->description = xml.readElementText();
                    depth--;
                }
                else if (elementName == "packageId")
                {
                    // 只在顶层读取 packageId
                    QString packageId = xml.readElementText();
                    mod->packageId = packageId;
                    mod->identifier = packageId;
                    depth--;
                }
                else if (elementName == "url")
                {
                    mod->url = xml.readElementText();
                    depth--;
                }
                else if (elementName == "supportedVersions")
                {
                    parseSupportedVersions(xml, mod);
                    depth--;
                }
                else if (elementName == "modDependencies")
                {
                    parseModDependencies(xml, mod);
                    depth--;
                }
                else if (elementName == "modDependenciesByVersion")
                {
                    parseModDependenciesByVersion(xml, mod);
                    depth--;
                }
                else if (elementName == "loadBefore")
                {
                    parseLoadBefore(xml, mod);
                    depth--;
                }
                else if (elementName == "loadAfter")
                {
                    parseLoadAfter(xml, mod);
                    depth--;
                }
                else if (elementName == "loadBeforeByVersion")
                {
                    parseLoadBeforeByVersion(xml, mod);
                    depth--;
                }
                else if (elementName == "loadAfterByVersion")
                {
                    parseLoadAfterByVersion(xml, mod);
                    depth--;
                }
                else if (elementName == "forceLoadBefore")
                {
                    parseForceLoadBefore(xml, mod);
                    depth--;
                }
                else if (elementName == "forceLoadAfter")
                {
                    parseForceLoadAfter(xml, mod);
                    depth--;
                }
                else if (elementName == "incompatibleWith")
                {
                    parseIncompatibleWith(xml, mod);
                    depth--;
                }
                else if (elementName == "incompatibleWithByVersion")
                {
                    parseIncompatibleWithByVersion(xml, mod);
                    depth--;
                }
            }
        }
        else if (token == QXmlStreamReader::EndElement)
        {
            depth--;
        }
    }

    file.close();

    return !xml.hasError() && mod->isValid();
}

void WorkshopScanner::parseSupportedVersions(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "supportedVersions"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addSupportedVersion(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseModDependencies(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "modDependencies"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            QString depPackageId = parseDependencyItem(xml);
            if (!depPackageId.isEmpty())
            {
                mod->addDependency(depPackageId);
            }
        }
    }
}

void WorkshopScanner::parseModDependenciesByVersion(QXmlStreamReader &xml, ModItem *mod)
{
    // 跳过整个 modDependenciesByVersion 块，收集所有依赖
    QSet<QString> allDependencies;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "modDependenciesByVersion"))
    {
        xml.readNext();

        // 处理版本块（如 v1.2, v1.3 等）
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v"))
        {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag))
            {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li")
                {
                    QString depPackageId = parseDependencyItem(xml);
                    if (!depPackageId.isEmpty())
                    {
                        allDependencies.insert(depPackageId);
                    }
                }
            }
        }
    }

    // 将所有收集的依赖添加到 mod
    for (const QString &dep : allDependencies)
    {
        mod->addDependency(dep);
    }
}

QString WorkshopScanner::parseDependencyItem(QXmlStreamReader &xml)
{
    QString packageId;

    // 读取 <li> 的内容，可能是直接的文本或包含 <packageId> 子元素
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "li"))
    {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "packageId")
            {
                packageId = xml.readElementText();
                // 找到 packageId 后可以跳出
                break;
            }
        }
        else if (xml.tokenType() == QXmlStreamReader::Characters)
        {
            // 如果是直接文本（旧格式）
            QString text = xml.text().toString().trimmed();
            if (!text.isEmpty())
            {
                packageId = text;
            }
        }
    }

    // 确保读取到 </li>
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "li"))
    {
        xml.readNext();
    }

    return packageId;
}

void WorkshopScanner::parseLoadBefore(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadBefore"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addLoadBefore(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseLoadAfter(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadAfter"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addLoadAfter(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseLoadBeforeByVersion(QXmlStreamReader &xml, ModItem *mod)
{
    QSet<QString> allLoadBefore;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadBeforeByVersion"))
    {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v"))
        {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag))
            {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li")
                {
                    allLoadBefore.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item : allLoadBefore)
    {
        mod->addLoadBefore(item);
    }
}

void WorkshopScanner::parseLoadAfterByVersion(QXmlStreamReader &xml, ModItem *mod)
{
    QSet<QString> allLoadAfter;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "loadAfterByVersion"))
    {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v"))
        {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag))
            {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li")
                {
                    allLoadAfter.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item : allLoadAfter)
    {
        mod->addLoadAfter(item);
    }
}

void WorkshopScanner::parseForceLoadBefore(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "forceLoadBefore"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addForceLoadBefore(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseForceLoadAfter(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "forceLoadAfter"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addForceLoadAfter(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseIncompatibleWith(QXmlStreamReader &xml, ModItem *mod)
{
    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "incompatibleWith"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name() == "li")
        {
            mod->addIncompatibleWith(xml.readElementText());
        }
    }
}

void WorkshopScanner::parseIncompatibleWithByVersion(QXmlStreamReader &xml, ModItem *mod)
{
    QSet<QString> allIncompatible;

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "incompatibleWithByVersion"))
    {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString().startsWith("v"))
        {
            QString versionTag = xml.name().toString();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                     xml.name() == versionTag))
            {
                xml.readNext();
                if (xml.tokenType() == QXmlStreamReader::StartElement &&
                    xml.name() == "li")
                {
                    allIncompatible.insert(xml.readElementText());
                }
            }
        }
    }

    for (const QString &item : allIncompatible)
    {
        mod->addIncompatibleWith(item);
    }
}

QString WorkshopScanner::getSteamPathFromRegistry()
{
#ifdef Q_OS_WIN
    // 从Windows注册表读取Steam安装路径
    QSettings steamReg("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
    QString steamPath = steamReg.value("SteamPath").toString();

    if (!steamPath.isEmpty())
    {
        // 将路径中的 / 替换为 \ （如果需要）
        return QDir::fromNativeSeparators(steamPath);
    }

    // 尝试另一个位置
    QSettings steamReg2("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
    steamPath = steamReg2.value("InstallPath").toString();

    if (!steamPath.isEmpty())
    {
        return QDir::fromNativeSeparators(steamPath);
    }
#endif

    return QString();
}
