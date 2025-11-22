#include "ModManager.h"
#include <QDir>

ModManager::ModManager()
    : m_workshopScanner(new WorkshopScanner()),
      m_dlcScanner(new OfficialDLCScanner())
{
}

ModManager::ModManager(const QString &steamPath)
    : m_steamPath(steamPath),
      m_workshopScanner(new WorkshopScanner()),
      m_dlcScanner(new OfficialDLCScanner())
{
    // 设置工坊路径
    QString workshopPath = QDir(steamPath).absoluteFilePath("steamapps/workshop/content/294100");
    m_workshopScanner->setWorkshopPath(workshopPath);

    // 默认游戏安装路径（假设在Steam标准位置）
    m_gameInstallPath = QDir(steamPath).absoluteFilePath("steamapps/common/RimWorld");

    // 设置DLC路径
    QString dataPath = OfficialDLCScanner::getDefaultDataPath(m_gameInstallPath);
    m_dlcScanner->setDataPath(dataPath);
}

ModManager::~ModManager()
{
    delete m_workshopScanner;
    delete m_dlcScanner;
}

void ModManager::setSteamPath(const QString &steamPath)
{
    m_steamPath = steamPath;

    // 更新扫描器路径
    QString workshopPath = QDir(steamPath).absoluteFilePath("steamapps/workshop/content/294100");
    m_workshopScanner->setWorkshopPath(workshopPath);

    // 默认游戏安装路径（假设在Steam标准位置）
    m_gameInstallPath = QDir(steamPath).absoluteFilePath("steamapps/common/RimWorld");

    QString dataPath = OfficialDLCScanner::getDefaultDataPath(m_gameInstallPath);
    m_dlcScanner->setDataPath(dataPath);
}

void ModManager::setGameInstallPath(const QString &gameInstallPath)
{
    m_gameInstallPath = gameInstallPath;

    // 更新DLC扫描器路径
    QString dataPath = OfficialDLCScanner::getDefaultDataPath(gameInstallPath);
    m_dlcScanner->setDataPath(dataPath);
}

bool ModManager::scanAll()
{
    clear();

    bool workshopSuccess = scanWorkshopMods();
    bool dlcSuccess = scanOfficialDLCs();

    // 重建统一映射
    for (ModItem *mod : m_workshopScanner->getScannedMods())
    {
        m_packageIdMap[mod->packageId] = mod;
    }

    for (ModItem *dlc : m_dlcScanner->getScannedDLCs())
    {
        m_packageIdMap[dlc->packageId] = dlc;
    }

    return workshopSuccess || dlcSuccess;
}

bool ModManager::scanWorkshopMods()
{
    return m_workshopScanner->scanAllMods();
}

bool ModManager::scanOfficialDLCs()
{
    return m_dlcScanner->scanAllDLCs();
}

QList<ModItem *> ModManager::getAllMods() const
{
    QList<ModItem *> allMods;

    // 先添加官方DLC
    allMods.append(m_dlcScanner->getScannedDLCs());

    // 再添加工坊Mod
    allMods.append(m_workshopScanner->getScannedMods());

    return allMods;
}

QList<ModItem *> ModManager::getWorkshopMods() const
{
    return m_workshopScanner->getScannedMods();
}

QList<ModItem *> ModManager::getOfficialDLCs() const
{
    return m_dlcScanner->getScannedDLCs();
}

ModItem *ModManager::findModByPackageId(const QString &packageId) const
{
    return m_packageIdMap.value(packageId, nullptr);
}

bool ModManager::isOfficialDLC(const QString &packageId) const
{
    ModItem *mod = findModByPackageId(packageId);
    return mod != nullptr && mod->isOfficialDLC;
}

void ModManager::clear()
{
    m_workshopScanner->clear();
    m_dlcScanner->clear();
    m_packageIdMap.clear();
}
