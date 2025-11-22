#include "data/ModConfigManager.h"
#include "data/ModItem.h"
#include "data/ModManager.h"
#include "data/ModTypeManager.h"
#include "data/UserDataManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Steam 路径
    QString steamPath = "D:/Steam";
    QString gameInstallPath = steamPath + "/steamapps/common/RimWorld";

    qDebug() << "========== 开始测试 ==========\n";
    qDebug() << "Steam路径:" << steamPath;
    qDebug() << "游戏安装路径:" << gameInstallPath;
    qDebug() << "";

    // 0. 测试 ModTypeManager - Mod类型管理
    qDebug() << "=== 测试 ModTypeManager ===";
    ModTypeManager typeManager;

    qDebug() << "默认Mod类型:";
    QStringList defaultTypes = ModTypeManager::getDefaultTypes();
    for (const QString &type : defaultTypes)
    {
        qDebug() << "  -" << type;
    }

    // 添加自定义类型
    typeManager.addCustomType("美化mod");
    typeManager.addCustomType("剧情mod");

    qDebug() << "\n添加自定义类型后的所有类型:";
    QStringList allTypes = typeManager.getAllTypes();
    for (const QString &type : allTypes)
    {
        QString typeLabel = typeManager.isDefaultType(type) ? "[默认]" : "[自定义]";
        qDebug() << "  " << typeLabel << type;
    }

    qDebug() << "\n";

    // 1. 测试 ModConfigManager - 读取当前加载的Mod列表
    qDebug() << "=== 测试 ModConfigManager ===";
    ModConfigManager configManager;

    if (configManager.loadConfig())
    {
        qDebug() << "成功加载 ModsConfig.xml";
        qDebug() << "游戏版本:" << configManager.getVersion();
        qDebug() << "已知扩展包数量:" << configManager.getKnownExpansions().size();

        QStringList knownExpansions = configManager.getKnownExpansions();
        if (!knownExpansions.isEmpty())
        {
            qDebug() << "已知扩展包:";
            for (const QString &expansion : knownExpansions)
            {
                qDebug() << "  -" << expansion;
            }
        }

        QStringList activeMods = configManager.getActiveMods();
        qDebug() << "\n当前激活的Mod数量:" << activeMods.size();
        qDebug() << "当前加载的Mod列表（显示前10个）:";
        for (int i = 0; i < qMin(10, activeMods.size()); ++i)
        {
            qDebug() << QString("  [%1] %2").arg(i + 1).arg(activeMods[i]);
        }

        // 测试XML生成是否正确保留信息
        qDebug() << "\n\n=== 测试XML生成 ===";
        QString testPath = "test_ModsConfig.xml";
        if (configManager.saveConfig(testPath))
        {
            qDebug() << "成功保存测试配置文件到:" << testPath;
            qDebug() << "请检查该文件是否保留了version和knownExpansions";

            // 重新加载测试
            ModConfigManager testManager(testPath);
            if (testManager.loadConfig())
            {
                qDebug() << "\n重新加载测试文件验证:";
                qDebug() << "  版本一致:" << (testManager.getVersion() == configManager.getVersion() ? "是" : "否");
                qDebug() << "  扩展包数量一致:" << (testManager.getKnownExpansions().size() == configManager.getKnownExpansions().size() ? "是" : "否");
                qDebug() << "  激活Mod数量一致:" << (testManager.getActiveMods().size() == configManager.getActiveMods().size() ? "是" : "否");
            }
        }
        else
        {
            qDebug() << "无法保存测试配置文件";
        }
    }
    else
    {
        qDebug() << "无法加载 ModsConfig.xml，请检查路径:" << configManager.getConfigPath();
    }

    qDebug() << "\n";

    // 2. 测试 ModManager - 扫描所有Mod（包括工坊和官方DLC）
    qDebug() << "=== 测试 ModManager ===";

    ModManager modManager(steamPath);

    // 显示路径信息
    qDebug() << "当前配置路径:";
    qDebug() << "  Steam路径:" << modManager.getSteamPath();
    qDebug() << "  游戏安装路径:" << modManager.getGameInstallPath();

    if (modManager.scanAll())
    {
        qDebug() << "\n成功扫描所有Mod";

        // 显示官方DLC
        QList<ModItem *> dlcs = modManager.getOfficialDLCs();
        qDebug() << "\n官方DLC数量:" << dlcs.size();
        if (!dlcs.isEmpty())
        {
            qDebug() << "官方DLC列表:";
            for (int i = 0; i < dlcs.size(); ++i)
            {
                const ModItem *dlc = dlcs[i];
                qDebug() << QString("  [DLC-%1] %2 (PackageId: %3)")
                                .arg(i + 1)
                                .arg(dlc->name)
                                .arg(dlc->packageId);
                qDebug() << QString("    路径: %1").arg(dlc->sourcePath);
            }
        }

        // 显示工坊Mod
        QList<ModItem *> mods = modManager.getWorkshopMods();
        qDebug() << "\n工坊Mod数量:" << mods.size();

        // 只显示前5个作为示例
        int displayCount = qMin(5, mods.size());
        if (displayCount > 0)
        {
            qDebug() << QString("\n工坊Mod列表（显示前%1个）:").arg(displayCount);
            for (int i = 0; i < displayCount; ++i)
            {
                const ModItem *mod = mods[i];
                qDebug() << QString("\n[Mod-%1] %2").arg(i + 1).arg(mod->name);
                qDebug() << QString("  PackageId: %1").arg(mod->packageId);
                qDebug() << QString("  作者: %1").arg(mod->author);
                qDebug() << QString("  官方DLC: %1").arg(mod->isOfficialDLC ? "是" : "否");
                qDebug() << QString("  路径: %1").arg(mod->sourcePath);

                if (!mod->supportedVersions.isEmpty())
                {
                    qDebug() << QString("  支持版本: %1").arg(mod->supportedVersions.join(", "));
                }

                if (!mod->dependencies.isEmpty())
                {
                    qDebug() << QString("  依赖: %1").arg(mod->dependencies.join(", "));
                }
            }
        }

        // 演示区分DLC和普通Mod
        qDebug() << "\n\n=== 演示区分DLC和Mod ===";
        QList<ModItem *> allMods = modManager.getAllMods();
        int dlcCount = 0;
        int modCount = 0;

        for (const ModItem *item : allMods)
        {
            if (item->isOfficialDLC)
            {
                dlcCount++;
            }
            else
            {
                modCount++;
            }
        }

        qDebug() << QString("总计: %1 个Mod，其中:").arg(allMods.size());
        qDebug() << QString("  - 官方DLC: %1 个").arg(dlcCount);
        qDebug() << QString("  - 工坊Mod: %1 个").arg(modCount);
    }
    else
    {
        qDebug() << "无法扫描Mod目录，请检查Steam路径是否正确";
    }

    qDebug() << "\n";

    // 3. 测试 UserDataManager - 用户数据管理
    qDebug() << "=== 测试 UserDataManager ===";
    UserDataManager userDataManager;

    qDebug() << "用户数据目录:" << UserDataManager::getUserDataPath();
    qDebug() << "Mod数据目录:" << UserDataManager::getModDataPath();
    qDebug() << "Mod列表目录:" << UserDataManager::getModListPath();

    // 测试自定义类型
    qDebug() << "\n--- 测试自定义类型 ---";
    userDataManager.addCustomType("美化mod");
    userDataManager.addCustomType("剧情mod");
    userDataManager.addCustomType("战斗增强");

    QStringList customTypes = userDataManager.getAllCustomTypes();
    qDebug() << "自定义类型列表:";
    for (const QString &type : customTypes)
    {
        qDebug() << "  -" << type;
    }

    // 测试Mod类型和备注设置
    qDebug() << "\n--- 测试Mod类型和备注 ---";
    userDataManager.setModType("brrainz.harmony", "前置框架");
    userDataManager.setModType("ludeon.rimworld", "核心");
    userDataManager.setModRemark("brrainz.harmony", "Harmony是最重要的前置mod");
    userDataManager.setModRemark("ludeon.rimworld", "RimWorld核心游戏");

    qDebug() << "设置的类型:";
    qDebug() << "  brrainz.harmony:" << userDataManager.getModType("brrainz.harmony");
    qDebug() << "  ludeon.rimworld:" << userDataManager.getModType("ludeon.rimworld");

    qDebug() << "设置的备注:";
    qDebug() << "  brrainz.harmony:" << userDataManager.getModRemark("brrainz.harmony");
    qDebug() << "  ludeon.rimworld:" << userDataManager.getModRemark("ludeon.rimworld");

    // 保存数据
    qDebug() << "\n--- 保存用户数据 ---";
    if (userDataManager.saveAll())
    {
        qDebug() << "用户数据保存成功";
    }
    else
    {
        qDebug() << "用户数据保存失败";
    }

    // 测试加载数据
    qDebug() << "\n--- 重新加载用户数据 ---";
    UserDataManager testManager;
    if (testManager.loadAll())
    {
        qDebug() << "用户数据加载成功";
        qDebug() << "加载的自定义类型数量:" << testManager.getAllCustomTypes().size();
        qDebug() << "验证数据一致性:";
        qDebug() << "  brrainz.harmony类型:" << testManager.getModType("brrainz.harmony");
        qDebug() << "  ludeon.rimworld备注:" << testManager.getModRemark("ludeon.rimworld");
    }
    else
    {
        qDebug() << "用户数据加载失败";
    }

    // 测试Mod列表保存
    qDebug() << "\n--- 测试Mod列表保存 ---";
    if (configManager.loadConfig())
    {
        QString testListPath = QDir(UserDataManager::getModListPath()).absoluteFilePath("test_modlist.xml");
        bool saveSuccess = userDataManager.saveModListToPath(
            testListPath,
            configManager.getActiveMods(),
            configManager.getKnownExpansions(),
            configManager.getVersion(),
            configManager.getOtherFields());

        if (saveSuccess)
        {
            qDebug() << "Mod列表保存成功:" << testListPath;
        }
        else
        {
            qDebug() << "Mod列表保存失败";
        }

        // 列出所有保存的Mod列表
        QStringList savedLists = userDataManager.listSavedModLists();
        qDebug() << "\n已保存的Mod列表 (" << savedLists.size() << "个):";
        for (const QString &listName : savedLists)
        {
            qDebug() << "  -" << listName;
        }
    }

    qDebug() << "\n========== 测试结束 ==========";

    return 0;
}
