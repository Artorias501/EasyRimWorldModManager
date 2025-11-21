#include "data/ModConfigManager.h"
#include "data/ModItem.h"
#include "data/WorkshopScanner.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Steam 路径
    QString steamPath = "D:/Steam";
    QString workshopPath = steamPath + "/steamapps/workshop/content/294100";

    qDebug() << "========== 开始测试 ==========\n";

    // 1. 测试 ModConfigManager - 读取当前加载的Mod列表
    qDebug() << "=== 测试 ModConfigManager ===";
    ModConfigManager configManager;

    if (configManager.loadConfig())
    {
        qDebug() << "成功加载 ModsConfig.xml";
        qDebug() << "游戏版本:" << configManager.getVersion();

        QStringList activeMods = configManager.getActiveMods();
        qDebug() << "\n当前激活的Mod数量:" << activeMods.size();
        qDebug() << "当前加载的Mod列表:";
        for (int i = 0; i < activeMods.size(); ++i)
        {
            qDebug() << QString("  [%1] %2").arg(i + 1).arg(activeMods[i]);
        }
    }
    else
    {
        qDebug() << "无法加载 ModsConfig.xml，请检查路径:" << configManager.getConfigPath();
    }

    qDebug() << "\n";

    // 2. 测试 WorkshopScanner - 扫描所有Steam创意工坊Mod
    qDebug() << "=== 测试 WorkshopScanner ===";
    qDebug() << "工坊路径:" << workshopPath;

    WorkshopScanner scanner(workshopPath);

    if (scanner.scanAllMods())
    {
        QVector<ModItem *> mods = scanner.getScannedMods();
        qDebug() << "成功扫描工坊目录";
        qDebug() << "扫描到的Mod数量:" << mods.size();

        qDebug() << "\n所有Mod列表:";
        for (int i = 0; i < mods.size(); ++i)
        {
            const ModItem *mod = mods[i];
            qDebug() << QString("\n[%1] %2").arg(i + 1).arg(mod->name);
            qDebug() << QString("  PackageId: %1").arg(mod->packageId);
            qDebug() << QString("  作者: %1").arg(mod->author);

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
    else
    {
        qDebug() << "无法扫描工坊目录，请检查路径是否正确";
    }

    qDebug() << "\n========== 测试结束 ==========";

    return 0;
}