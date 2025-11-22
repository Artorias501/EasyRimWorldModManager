#include "data/PathConfig.h"
#include "ui/MainWindow.h"
#include "ui/PathSettingsDialog.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("EasyRimWorldModManager");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("RimWorld Tools");

    // 检查路径配置是否存在
    PathConfig pathConfig;
    if (!PathConfig::configExists())
    {
        // 首次运行，显示路径设置对话框
        PathSettingsDialog dialog;
        dialog.setWindowTitle("首次运行 - 路径配置");

        if (dialog.exec() != QDialog::Accepted)
        {
            // 用户取消了配置，退出程序
            QMessageBox::warning(nullptr, "无法启动", "需要配置路径才能使用程序");
            return 0;
        }

        // 保存配置
        pathConfig = dialog.getPathConfig();
        if (!pathConfig.save())
        {
            QMessageBox::critical(nullptr, "错误", "无法保存路径配置");
            return 1;
        }
    }
    else
    {
        // 加载现有配置
        if (!pathConfig.load())
        {
            QMessageBox::critical(nullptr, "错误", "无法加载路径配置，请重新设置");

            PathSettingsDialog dialog;
            if (dialog.exec() != QDialog::Accepted)
            {
                return 0;
            }

            pathConfig = dialog.getPathConfig();
            if (!pathConfig.save())
            {
                QMessageBox::critical(nullptr, "错误", "无法保存路径配置");
                return 1;
            }
        }
    }

    // 创建并显示主窗口
    MainWindow window;
    window.show();

    // 自动开始扫描Mod
    QMetaObject::invokeMethod(&window, "startScan", Qt::QueuedConnection);

    return app.exec();
}
