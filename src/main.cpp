#include "data/ModConfigManager.h"
#include "data/ModItem.h"
#include "data/ModManager.h"
#include "data/ModTypeManager.h"
#include "data/UserDataManager.h"
#include "test/test_functions.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "========================================";
    qDebug() << "  EasyRimWorldModManager - 功能测试";
    qDebug() << "========================================";
    qDebug() << "";
    qDebug() << "本测试将执行以下操作：";
    qDebug() << "  1. 扫描所有已安装的mod和DLC";
    qDebug() << "  2. 读取当前游戏加载的mod列表";
    qDebug() << "  3. 修改mod列表排序并添加/移除DLC";
    qDebug() << "  4. 创建空白加载列表并添加mod";
    qDebug() << "";
    qDebug() << "⚠️  注意：所有配置都保存到UserData目录";
    qDebug() << "   不会修改游戏的实际配置文件！";
    qDebug() << "";

    // 运行所有测试
    runAllTests();

    qDebug() << "";
    qDebug() << "========================================";
    qDebug() << "  测试完成";
    qDebug() << "========================================";

    return 0;
}
