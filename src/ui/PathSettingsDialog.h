#ifndef PATHSETTINGSDIALOG_H
#define PATHSETTINGSDIALOG_H

#include "../data/PathConfig.h"
#include <QDialog>

namespace Ui
{
    class PathSettingsDialog;
}

/**
 * @brief 路径设置对话框
 *
 * 用于配置游戏相关的路径：
 * - Steam 安装路径
 * - 游戏安装路径
 * - 用户存档路径
 */
class PathSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathSettingsDialog(QWidget *parent = nullptr);
    explicit PathSettingsDialog(PathConfig *config, QWidget *parent = nullptr);
    ~PathSettingsDialog();

    // 获取配置的路径
    PathConfig getPathConfig() const;

    // 设置路径（用于编辑现有配置）
    void setPathConfig(const PathConfig &config);

protected:
    void accept() override;

private slots:
    void onBrowseSteamPath();
    void onBrowseGamePath();
    void onBrowseSavePath();

private:
    Ui::PathSettingsDialog *ui;
    PathConfig *m_pathConfig;

    void setupConnections();
    void loadDefaultPaths();
    bool validatePaths();
};

#endif // PATHSETTINGSDIALOG_H
