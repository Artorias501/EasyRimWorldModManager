#include "ModManager.h"
#include <QDebug>
#include <QDir>

ModManager::ModManager()
    : m_workshopScanner(new WorkshopScanner()),
      m_dlcScanner(new OfficialDLCScanner()),
      m_userDataManager(new UserDataManager())
{
    // 初始化用户数据目录
    UserDataManager::initializeDirectories();

    // 加载用户数据
    m_userDataManager->loadAll();
}

ModManager::ModManager(const QString &steamPath)
    : m_steamPath(steamPath),
      m_workshopScanner(new WorkshopScanner()),
      m_dlcScanner(new OfficialDLCScanner()),
      m_userDataManager(new UserDataManager())
{
    // 初始化用户数据目录
    UserDataManager::initializeDirectories();

    // 加载用户数据
    m_userDataManager->loadAll();

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
    // 在销毁前保存用户数据
    saveModsToUserData();
    m_userDataManager->saveAll();

    // 删除扫描器
    delete m_workshopScanner;
    delete m_dlcScanner;
    delete m_userDataManager;

    // 清理缓存的Mod对象（扫描器内部的对象已被清理）
    m_cachedWorkshopMods.clear();
    m_cachedOfficialDLCs.clear();
    m_packageIdMap.clear();
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
    // 清除旧数据
    clear();

    // 扫描工坊Mod和官方DLC
    bool workshopSuccess = scanWorkshopMods();
    bool dlcSuccess = scanOfficialDLCs();

    // 从UserDataManager加载备注和类型到ModItem
    loadUserDataToMods();

    return workshopSuccess || dlcSuccess;
}

bool ModManager::scanWorkshopMods()
{
    // 使用扫描器扫描
    bool success = m_workshopScanner->scanAllMods();

    if (success)
    {
        // 缓存扫描结果
        m_cachedWorkshopMods = m_workshopScanner->getScannedMods();

        // 更新PackageId映射
        for (ModItem *mod : m_cachedWorkshopMods)
        {
            m_packageIdMap[mod->packageId] = mod;
        }

        qDebug() << "[ModManager] Scanned" << m_cachedWorkshopMods.size() << "workshop mods";
    }

    return success;
}

bool ModManager::scanOfficialDLCs()
{
    // 使用扫描器扫描
    bool success = m_dlcScanner->scanAllDLCs();

    if (success)
    {
        // 缓存扫描结果
        m_cachedOfficialDLCs = m_dlcScanner->getScannedDLCs();

        // 更新PackageId映射
        for (ModItem *dlc : m_cachedOfficialDLCs)
        {
            m_packageIdMap[dlc->packageId] = dlc;
        }

        qDebug() << "[ModManager] Scanned" << m_cachedOfficialDLCs.size() << "official DLCs";
    }

    return success;
}

QList<ModItem *> ModManager::getAllMods() const
{
    QList<ModItem *> allMods;

    // 先添加官方DLC
    allMods.append(m_cachedOfficialDLCs);

    // 再添加工坊Mod
    allMods.append(m_cachedWorkshopMods);

    return allMods;
}

QList<ModItem *> ModManager::getWorkshopMods() const
{
    return m_cachedWorkshopMods;
}

QList<ModItem *> ModManager::getOfficialDLCs() const
{
    return m_cachedOfficialDLCs;
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

void ModManager::loadUserDataToMods()
{
    int loadedCount = 0;

    // 遍历所有缓存的Mod
    QList<ModItem *> allMods = getAllMods();

    for (ModItem *mod : allMods)
    {
        if (!mod)
            continue;

        // 从UserDataManager加载类型
        QString type = m_userDataManager->getModType(mod->packageId);
        if (!type.isEmpty())
        {
            mod->type = type;
            loadedCount++;
        }

        // 从UserDataManager加载备注
        QString remark = m_userDataManager->getModRemark(mod->packageId);
        if (!remark.isEmpty())
        {
            mod->remark = remark;
            loadedCount++;
        }
    }

    qDebug() << "[ModManager] Loaded user data to" << loadedCount << "fields across" << allMods.size() << "mods";
}

void ModManager::saveModsToUserData()
{
    int savedCount = 0;

    // 遍历所有缓存的Mod
    QList<ModItem *> allMods = getAllMods();

    for (ModItem *mod : allMods)
    {
        if (!mod)
            continue;

        // 保存类型到UserDataManager
        if (!mod->type.isEmpty())
        {
            m_userDataManager->setModType(mod->packageId, mod->type);
            savedCount++;
        }

        // 保存备注到UserDataManager
        if (!mod->remark.isEmpty())
        {
            m_userDataManager->setModRemark(mod->packageId, mod->remark);
            savedCount++;
        }
    }

    qDebug() << "[ModManager] Saved" << savedCount << "user data fields from" << allMods.size() << "mods";

    // 持久化到文件
    m_userDataManager->saveModData();
}

void ModManager::clear()
{
    // 清理扫描器（这会删除内部的ModItem对象）
    m_workshopScanner->clear();
    m_dlcScanner->clear();

    // 清理缓存（只清空指针，不delete，因为对象已被扫描器删除）
    m_cachedWorkshopMods.clear();
    m_cachedOfficialDLCs.clear();
    m_packageIdMap.clear();
}
