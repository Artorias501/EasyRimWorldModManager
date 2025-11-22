#include "ModConfigManager.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

ModConfigManager::ModConfigManager()
    : m_configPath(getDefaultConfigPath())
{
}

ModConfigManager::ModConfigManager(const QString &configPath)
    : m_configPath(configPath)
{
}

bool ModConfigManager::loadConfig()
{
    return loadConfig(m_configPath);
}

bool ModConfigManager::loadConfig(const QString &configPath)
{
    m_configPath = configPath;

    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QString xmlContent = file.readAll();
    file.close();

    return parseXml(xmlContent);
}

bool ModConfigManager::saveConfig()
{
    return saveConfig(m_configPath);
}

bool ModConfigManager::saveConfig(const QString &configPath)
{
    m_configPath = configPath;

    // 确保目录存在
    QFileInfo fileInfo(configPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(configPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QString xmlContent = generateXml();
    file.write(xmlContent.toUtf8());
    file.close();

    return true;
}

void ModConfigManager::setActiveMods(const QStringList &mods)
{
    m_activeMods = mods;
}

void ModConfigManager::addMod(const QString &modId)
{
    if (!modId.isEmpty() && !m_activeMods.contains(modId))
    {
        m_activeMods.append(modId);
    }
}

void ModConfigManager::removeMod(const QString &modId)
{
    m_activeMods.removeAll(modId);
}

void ModConfigManager::insertMod(int index, const QString &modId)
{
    if (!modId.isEmpty() && !m_activeMods.contains(modId))
    {
        if (index < 0 || index > m_activeMods.size())
        {
            m_activeMods.append(modId);
        }
        else
        {
            m_activeMods.insert(index, modId);
        }
    }
}

bool ModConfigManager::moveModUp(const QString &modId)
{
    int index = m_activeMods.indexOf(modId);
    if (index > 0)
    {
        m_activeMods.move(index, index - 1);
        return true;
    }
    return false;
}

bool ModConfigManager::moveModDown(const QString &modId)
{
    int index = m_activeMods.indexOf(modId);
    if (index >= 0 && index < m_activeMods.size() - 1)
    {
        m_activeMods.move(index, index + 1);
        return true;
    }
    return false;
}

bool ModConfigManager::moveModToPosition(const QString &modId, int newPosition)
{
    int currentIndex = m_activeMods.indexOf(modId);
    if (currentIndex >= 0 && newPosition >= 0 && newPosition < m_activeMods.size())
    {
        m_activeMods.move(currentIndex, newPosition);
        return true;
    }
    return false;
}

bool ModConfigManager::isModActive(const QString &modId) const
{
    return m_activeMods.contains(modId);
}

int ModConfigManager::getModPosition(const QString &modId) const
{
    return m_activeMods.indexOf(modId);
}

QString ModConfigManager::getDefaultConfigPath()
{
    // C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    // GenericDataLocation 在 Windows 上返回 AppData\Local，我们需要 AppData\LocalLow
    // 所以需要手动构建路径
    QDir appDataDir(appDataPath);
    appDataDir.cdUp(); // 回到 AppData
    QString localLowPath = appDataDir.absoluteFilePath("LocalLow");

    return QDir(localLowPath).absoluteFilePath("Ludeon Studios/RimWorld by Ludeon Studios/Config/ModsConfig.xml");
}

bool ModConfigManager::parseXml(const QString &xmlContent)
{
    QXmlStreamReader xml(xmlContent);

    m_activeMods.clear();
    m_knownExpansions.clear();
    m_version.clear();
    m_otherFields.clear();

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            QString elementName = xml.name().toString();

            // 跳过根元素
            if (elementName == "ModsConfigData")
            {
                continue;
            }

            if (elementName == "version")
            {
                m_version = xml.readElementText();
            }
            else if (elementName == "activeMods")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "activeMods"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        QString modId = xml.readElementText();
                        if (!modId.isEmpty())
                        {
                            m_activeMods.append(modId);
                        }
                    }
                }
            }
            else if (elementName == "knownExpansions")
            {
                // 读取所有 <li> 元素
                while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
                         xml.name() == "knownExpansions"))
                {
                    xml.readNext();
                    if (xml.tokenType() == QXmlStreamReader::StartElement &&
                        xml.name() == "li")
                    {
                        QString expansionId = xml.readElementText();
                        if (!expansionId.isEmpty())
                        {
                            m_knownExpansions.append(expansionId);
                        }
                    }
                }
            }
            else
            {
                // 保存其他未知字段
                QString value = xml.readElementText();
                m_otherFields[elementName] = value;
            }
        }
    }

    return !xml.hasError();
}

QString ModConfigManager::generateXml() const
{
    QString xml;
    QXmlStreamWriter writer(&xml);

    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument("1.0", true); // 明确指定版本和standalone
    writer.writeStartElement("ModsConfigData");

    // 写入版本（必须保留原始版本信息）
    if (!m_version.isEmpty())
    {
        writer.writeTextElement("version", m_version);
    }

    // 写入激活的Mod列表（包括核心、DLC和创意工坊mod）
    writer.writeStartElement("activeMods");
    for (const QString &modId : m_activeMods)
    {
        writer.writeTextElement("li", modId);
    }
    writer.writeEndElement(); // activeMods

    // 写入已知扩展包（总是写入，即使为空，保持与原文件结构一致）
    writer.writeStartElement("knownExpansions");
    for (const QString &expansionId : m_knownExpansions)
    {
        writer.writeTextElement("li", expansionId);
    }
    writer.writeEndElement(); // knownExpansions

    // 写入其他保存的字段（保持原样）
    for (auto it = m_otherFields.begin(); it != m_otherFields.end(); ++it)
    {
        writer.writeTextElement(it.key(), it.value());
    }

    writer.writeEndElement(); // ModsConfigData
    writer.writeEndDocument();

    return xml;
}
