#include "OfficialDLCScanner.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>

OfficialDLCScanner::OfficialDLCScanner()
{
}

OfficialDLCScanner::OfficialDLCScanner(const QString &dataPath)
    : m_dataPath(dataPath)
{
}

void OfficialDLCScanner::setDataPath(const QString &path)
{
    m_dataPath = path;
}

bool OfficialDLCScanner::scanAllDLCs()
{
    clear();

    QDir dataDir(m_dataPath);
    if (!dataDir.exists())
    {
        return false;
    }

    // 获取所有DLC目录
    QStringList dlcDirs = dataDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &dlcDirName : dlcDirs)
    {
        QString dlcDirPath = dataDir.absoluteFilePath(dlcDirName);
        ModItem *dlc = scanDLCDirectory(dlcDirPath);

        if (dlc && dlc->isValid())
        {
            m_scannedDLCs.append(dlc);
            m_packageIdMap[dlc->packageId] = dlc;
        }
        else if (dlc)
        {
            delete dlc; // 删除无效的DLC对象
        }
    }

    return true;
}

ModItem *OfficialDLCScanner::findDLCByPackageId(const QString &packageId) const
{
    return m_packageIdMap.value(packageId, nullptr);
}

void OfficialDLCScanner::clear()
{
    // 删除所有DLC对象
    qDeleteAll(m_scannedDLCs);
    m_scannedDLCs.clear();
    m_packageIdMap.clear();
}

QString OfficialDLCScanner::getDefaultDataPath(const QString &gameInstallPath)
{
    if (gameInstallPath.isEmpty())
    {
        return QString();
    }

    // 游戏安装路径下的Data文件夹
    return QDir(gameInstallPath).absoluteFilePath("Data");
}

ModItem *OfficialDLCScanner::scanDLCDirectory(const QString &dlcDirPath)
{
    // 检查About.xml文件是否存在
    QString aboutXmlPath = QDir(dlcDirPath).absoluteFilePath("About/About.xml");

    if (!QFile::exists(aboutXmlPath))
    {
        return nullptr;
    }

    ModItem *dlc = new ModItem();

    if (!parseAboutXml(dlc, aboutXmlPath))
    {
        delete dlc;
        return nullptr;
    }

    // 标记为官方DLC
    dlc->isOfficialDLC = true;
    dlc->sourcePath = dlcDirPath;
    dlc->type = "DLC"; // 默认类型设置为DLC

    return dlc;
}

bool OfficialDLCScanner::parseAboutXml(ModItem *dlc, const QString &aboutXmlPath)
{
    QFile file(aboutXmlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            QString elementName = xml.name().toString();

            if (elementName == "name")
            {
                dlc->name = xml.readElementText();
            }
            else if (elementName == "author")
            {
                dlc->author = xml.readElementText();
            }
            else if (elementName == "description")
            {
                dlc->description = xml.readElementText();
            }
            else if (elementName == "packageId")
            {
                QString packageId = xml.readElementText();
                dlc->packageId = packageId;
                dlc->identifier = packageId;
            }
            else if (elementName == "url")
            {
                dlc->url = xml.readElementText();
            }
            else if (elementName == "supportedVersions")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "supportedVersions"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        dlc->addSupportedVersion(xml.readElementText());
                    }
                }
            }
            else if (elementName == "modDependencies")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "modDependencies"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        // 在<li>中查找<packageId>
                        while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                                 xml.name() == "li"))
                        {
                            xml.readNext();
                            if (xml.tokenType() == QXmlStreamReader::StartElement &&
                                xml.name() == "packageId")
                            {
                                dlc->addDependency(xml.readElementText());
                            }
                        }
                    }
                }
            }
            else if (elementName == "loadBefore")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "loadBefore"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        dlc->addLoadBefore(xml.readElementText());
                    }
                }
            }
            else if (elementName == "loadAfter")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "loadAfter"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        dlc->addLoadAfter(xml.readElementText());
                    }
                }
            }
            else if (elementName == "incompatibleWith")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "incompatibleWith"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        dlc->addIncompatibleWith(xml.readElementText());
                    }
                }
            }
        }
    }

    file.close();

    return !xml.hasError() && dlc->isValid();
}
