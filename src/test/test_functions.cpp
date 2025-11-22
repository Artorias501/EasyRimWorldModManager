#include "test_functions.h"
#include "../data/ModConfigManager.h"
#include "../data/ModManager.h"
#include "../data/UserDataManager.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>

// 辅助函数：打印分隔线
void printSeparator(const QString &title = "")
{
    qDebug() << "\n"
             << QString(60, '=');
    if (!title.isEmpty())
    {
        qDebug() << title;
        qDebug() << QString(60, '=');
    }
}

// 辅助函数：打印ModItem信息
void printModInfo(const ModItem *mod, int index = -1)
{
    if (!mod)
        return;

    QString prefix = (index >= 0) ? QString("[%1] ").arg(index) : "    ";
    qDebug() << prefix << mod->name;
    qDebug() << "        PackageId:" << mod->packageId;
    qDebug() << "        Type:" << mod->type;
    qDebug() << "        IsOfficialDLC:" << (mod->isOfficialDLC ? "Yes" : "No");
    qDebug() << "        Path:" << mod->sourcePath;
}

void test_ScanInstalledModsAndDLCs()
{
    printSeparator("测试1：扫描所有已安装的mod和DLC");

    // 创建ModManager
    ModManager modManager;

    // 检测Steam路径
    QString steamPath = WorkshopScanner::detectSteamPath();
    if (steamPath.isEmpty())
    {
        qDebug() << "⚠️  未能自动检测Steam路径，使用默认路径";
        steamPath = "C:/Program Files (x86)/Steam";
    }

    qDebug() << "Steam路径:" << steamPath;
    modManager.setSteamPath(steamPath);

    // 设置游戏安装路径
    QString gameInstallPath = QDir(steamPath).absoluteFilePath("steamapps/common/RimWorld");
    qDebug() << "游戏安装路径:" << gameInstallPath;
    modManager.setGameInstallPath(gameInstallPath);

    // 扫描所有mod
    qDebug() << "\n正在扫描...";
    if (!modManager.scanAll())
    {
        qDebug() << "❌ 扫描失败";
        return;
    }

    // 显示扫描结果
    QList<ModItem *> workshopMods = modManager.getWorkshopMods();
    QList<ModItem *> officialDLCs = modManager.getOfficialDLCs();

    qDebug() << "\n✅ 扫描完成";
    qDebug() << "   创意工坊mod数量:" << workshopMods.size();
    qDebug() << "   官方内容数量:" << officialDLCs.size();

    // 显示前5个创意工坊mod
    qDebug() << "\n前5个创意工坊mod:";
    for (int i = 0; i < qMin(5, workshopMods.size()); ++i)
    {
        printModInfo(workshopMods[i], i + 1);
    }

    // 显示所有官方内容
    qDebug() << "\n所有官方内容:";
    for (int i = 0; i < officialDLCs.size(); ++i)
    {
        printModInfo(officialDLCs[i], i + 1);
    }

    qDebug() << "\n✓ 测试1完成";
}

void test_ReadCurrentModList()
{
    printSeparator("测试2：读取当前游戏加载的mod列表");

    // 创建ModConfigManager
    ModConfigManager configManager;

    // 获取配置文件路径
    QString configPath = ModConfigManager::getDefaultConfigPath();
    qDebug() << "配置文件路径:" << configPath;

    // 检查文件是否存在
    if (!QFile::exists(configPath))
    {
        qDebug() << "❌ 配置文件不存在";
        qDebug() << "   请先运行一次游戏以生成配置文件";
        return;
    }

    // 读取配置
    qDebug() << "\n正在读取配置...";
    if (!configManager.loadConfig())
    {
        qDebug() << "❌ 读取失败";
        return;
    }

    // 显示配置信息
    qDebug() << "\n✅ 读取成功";
    qDebug() << "   游戏版本:" << configManager.getVersion();
    qDebug() << "   激活mod数量:" << configManager.getActiveMods().size();
    qDebug() << "   已知扩展包数量:" << configManager.getKnownExpansions().size();

    // 显示激活的mod列表
    QStringList activeMods = configManager.getActiveMods();
    qDebug() << "\n当前加载的mod列表:";
    for (int i = 0; i < qMin(10, activeMods.size()); ++i)
    {
        qDebug() << QString("  [%1] %2").arg(i + 1).arg(activeMods[i]);
    }
    if (activeMods.size() > 10)
    {
        qDebug() << "  ... 还有" << (activeMods.size() - 10) << "个mod";
    }

    // 显示已知扩展包
    QStringList knownExpansions = configManager.getKnownExpansions();
    qDebug() << "\n已知扩展包 (knownExpansions):";
    for (const QString &expansion : knownExpansions)
    {
        qDebug() << "  *" << expansion;
    }

    qDebug() << "\n✓ 测试2完成";
}

void test_ModifyModListAndDLCs()
{
    printSeparator("测试3：修改mod列表排序并添加/移除DLC");

    // 1. 读取当前配置
    ModConfigManager configManager;
    qDebug() << "1. 读取当前配置...";
    if (!configManager.loadConfig())
    {
        qDebug() << "❌ 读取失败";
        return;
    }
    qDebug() << "   ✓ 原始配置加载成功";
    qDebug() << "   - 激活mod数:" << configManager.getActiveMods().size();
    qDebug() << "   - knownExpansions数:" << configManager.getKnownExpansions().size();

    // 2. 扫描所有可用mod
    qDebug() << "\n2. 扫描可用mod...";
    ModManager modManager;
    QString steamPath = WorkshopScanner::detectSteamPath();
    if (steamPath.isEmpty())
    {
        steamPath = "C:/Program Files (x86)/Steam";
    }
    modManager.setSteamPath(steamPath);
    modManager.setGameInstallPath(QDir(steamPath).absoluteFilePath("steamapps/common/RimWorld"));

    if (!modManager.scanAll())
    {
        qDebug() << "❌ 扫描失败";
        return;
    }
    qDebug() << "   ✓ 扫描完成";

    // 3. 构建新的mod列表
    qDebug() << "\n3. 构建新的mod列表...";
    QList<ModItem *> allMods = modManager.getAllMods();
    QList<ModItem *> newModList;

    // 首先确保Core在最前面
    for (ModItem *mod : allMods)
    {
        if (mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0)
        {
            newModList.append(mod);
            qDebug() << "   ✓ 添加 Core:" << mod->packageId;
            break;
        }
    }

    // 添加一些创意工坊mod（前3个）
    qDebug() << "   添加创意工坊mod...";
    int workshopCount = 0;
    for (ModItem *mod : allMods)
    {
        if (!mod->isOfficialDLC && mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) != 0)
        {
            newModList.append(mod);
            qDebug() << "   ✓ 添加:" << mod->name;
            workshopCount++;
            if (workshopCount >= 3)
                break;
        }
    }

    // 添加官方DLC（测试knownExpansions）
    qDebug() << "   添加官方DLC...";
    for (ModItem *mod : allMods)
    {
        if (mod->isOfficialDLC)
        {
            newModList.append(mod);
            qDebug() << "   ✓ 添加 DLC:" << mod->name;
        }
    }

    // 4. 使用setActiveModsFromList设置（会自动生成knownExpansions）
    qDebug() << "\n4. 应用新的mod列表...";
    configManager.setActiveModsFromList(newModList);

    qDebug() << "   ✓ 新配置已生成";
    qDebug() << "   - 激活mod数:" << configManager.getActiveMods().size();
    qDebug() << "   - knownExpansions数:" << configManager.getKnownExpansions().size();

    // 显示knownExpansions内容
    qDebug() << "\n   knownExpansions 内容:";
    for (const QString &expansion : configManager.getKnownExpansions())
    {
        qDebug() << "     *" << expansion;
    }

    // 5. 保存到UserData
    qDebug() << "\n5. 保存配置到 UserData...";
    UserDataManager userDataManager;
    QString savePath = "UserData/ModList/test_modified_config.xml";

    // 确保目录存在
    QFileInfo fileInfo(savePath);
    QDir().mkpath(fileInfo.absolutePath());

    if (userDataManager.saveModListToPath(
            savePath,
            configManager.getActiveMods(),
            configManager.getKnownExpansions(),
            configManager.getVersion(),
            configManager.getOtherFields()))
    {
        qDebug() << "   ✓ 配置已保存到:" << savePath;
    }
    else
    {
        qDebug() << "   ❌ 保存失败";
    }

    // 6. 测试排序功能
    qDebug() << "\n6. 测试mod排序功能...";
    if (configManager.getActiveMods().size() >= 3)
    {
        QString firstMod = configManager.getActiveMods()[0];
        QString secondMod = configManager.getActiveMods()[1];

        qDebug() << "   移动前:";
        qDebug() << "     [0]" << firstMod;
        qDebug() << "     [1]" << secondMod;

        // 将第二个mod移动到第一个位置
        configManager.moveModToPosition(secondMod, 0);

        qDebug() << "   移动后:";
        qDebug() << "     [0]" << configManager.getActiveMods()[0];
        qDebug() << "     [1]" << configManager.getActiveMods()[1];

        // 保存排序后的配置
        QString sortedPath = "UserData/ModList/test_sorted_config.xml";
        userDataManager.saveModListToPath(
            sortedPath,
            configManager.getActiveMods(),
            configManager.getKnownExpansions(),
            configManager.getVersion(),
            configManager.getOtherFields());

        qDebug() << "   ✓ 排序后的配置已保存到:" << sortedPath;
    }

    qDebug() << "\n✓ 测试3完成";
}

void test_CreateEmptyModList()
{
    printSeparator("测试4：创建空白加载列表并添加mod");

    // 1. 创建空白配置
    ModConfigManager configManager;
    qDebug() << "1. 创建空白加载列表...";

    if (!configManager.loadConfigWithEmptyMods())
    {
        qDebug() << "❌ 创建失败（可能游戏配置文件不存在）";
        return;
    }

    qDebug() << "   ✓ 空白列表创建成功";
    qDebug() << "   - 游戏版本:" << configManager.getVersion();
    qDebug() << "   - activeMods数:" << configManager.getActiveMods().size() << "(应为0)";
    qDebug() << "   - knownExpansions数:" << configManager.getKnownExpansions().size() << "(应为0)";

    // 2. 扫描可用mod
    qDebug() << "\n2. 扫描可用mod...";
    ModManager modManager;
    QString steamPath = WorkshopScanner::detectSteamPath();
    if (steamPath.isEmpty())
    {
        steamPath = "C:/Program Files (x86)/Steam";
    }
    modManager.setSteamPath(steamPath);
    modManager.setGameInstallPath(QDir(steamPath).absoluteFilePath("steamapps/common/RimWorld"));

    if (!modManager.scanAll())
    {
        qDebug() << "❌ 扫描失败";
        return;
    }
    qDebug() << "   ✓ 扫描完成";

    // 3. 选择要添加的mod
    qDebug() << "\n3. 选择并添加mod...";
    QList<ModItem *> allMods = modManager.getAllMods();
    QList<ModItem *> selectedMods;

    // 添加Core
    for (ModItem *mod : allMods)
    {
        if (mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0)
        {
            selectedMods.append(mod);
            qDebug() << "   ✓ 选择 Core:" << mod->name;
            break;
        }
    }

    // 添加2个创意工坊mod
    qDebug() << "   选择创意工坊mod...";
    int workshopCount = 0;
    for (ModItem *mod : allMods)
    {
        if (!mod->isOfficialDLC && mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) != 0)
        {
            selectedMods.append(mod);
            qDebug() << "   ✓ 选择:" << mod->name;
            workshopCount++;
            if (workshopCount >= 2)
                break;
        }
    }

    // 添加1个官方DLC
    qDebug() << "   选择官方DLC...";
    for (ModItem *mod : allMods)
    {
        if (mod->isOfficialDLC)
        {
            selectedMods.append(mod);
            qDebug() << "   ✓ 选择 DLC:" << mod->name;
            break;
        }
    }

    // 4. 应用选择的mod
    qDebug() << "\n4. 应用选择的mod...";
    configManager.setActiveModsFromList(selectedMods);

    qDebug() << "   ✓ 已应用" << selectedMods.size() << "个mod";
    qDebug() << "   - activeMods数:" << configManager.getActiveMods().size();
    qDebug() << "   - knownExpansions数:" << configManager.getKnownExpansions().size();

    // 显示最终列表
    qDebug() << "\n   最终mod列表:";
    for (int i = 0; i < configManager.getActiveMods().size(); ++i)
    {
        qDebug() << QString("     [%1] %2").arg(i).arg(configManager.getActiveMods()[i]);
    }

    qDebug() << "\n   knownExpansions:";
    for (const QString &expansion : configManager.getKnownExpansions())
    {
        qDebug() << "     *" << expansion;
    }

    // 5. 测试排序
    qDebug() << "\n5. 测试mod排序...";
    if (configManager.getActiveMods().size() >= 2)
    {
        QString lastMod = configManager.getActiveMods().last();
        qDebug() << "   将" << lastMod << "移动到第1位";

        configManager.moveModToPosition(lastMod, 1);

        qDebug() << "   排序后的列表:";
        for (int i = 0; i < configManager.getActiveMods().size(); ++i)
        {
            qDebug() << QString("     [%1] %2").arg(i).arg(configManager.getActiveMods()[i]);
        }
    }

    // 6. 保存到UserData
    qDebug() << "\n6. 保存配置到 UserData...";
    UserDataManager userDataManager;
    QString savePath = "UserData/ModList/test_empty_start_config.xml";

    // 确保目录存在
    QFileInfo fileInfo(savePath);
    QDir().mkpath(fileInfo.absolutePath());

    if (userDataManager.saveModListToPath(
            savePath,
            configManager.getActiveMods(),
            configManager.getKnownExpansions(),
            configManager.getVersion(),
            configManager.getOtherFields()))
    {
        qDebug() << "   ✓ 配置已保存到:" << savePath;
    }
    else
    {
        qDebug() << "   ❌ 保存失败";
    }

    qDebug() << "\n✓ 测试4完成";
}

void runAllTests()
{
    printSeparator("开始运行所有测试");
    qDebug() << "测试时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "";

    // 运行所有测试
    test_ScanInstalledModsAndDLCs();
    test_ReadCurrentModList();
    test_ModifyModListAndDLCs();
    test_CreateEmptyModList();

    printSeparator("所有测试完成");
    qDebug() << "\n生成的文件:";
    qDebug() << "  - UserData/ModList/test_modified_config.xml";
    qDebug() << "  - UserData/ModList/test_sorted_config.xml";
    qDebug() << "  - UserData/ModList/test_empty_start_config.xml";
    qDebug() << "\n提示：所有配置都保存到了UserData目录，没有修改游戏的实际配置。";
}
