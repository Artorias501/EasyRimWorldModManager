#ifndef PATHCONFIG_H
#define PATHCONFIG_H

#include <QString>

/**
 * @brief 路径配置管理器
 *
 * 管理应用程序需要的各种路径配置：
 * - 游戏安装路径
 * - Steam 安装路径
 * - 用户存档路径
 *
 * 配置保存在 UserData/path.json 文件中
 */
class PathConfig
{
public:
    PathConfig();

    // ==================== 路径访问 ====================

    // 获取游戏安装路径
    QString getGameInstallPath() const { return m_gameInstallPath; }

    // 设置游戏安装路径
    void setGameInstallPath(const QString &path) { m_gameInstallPath = path; }

    // 获取 Steam 安装路径
    QString getSteamPath() const { return m_steamPath; }

    // 设置 Steam 安装路径
    void setSteamPath(const QString &path) { m_steamPath = path; }

    // 获取用户存档路径
    QString getUserSavePath() const { return m_userSavePath; }

    // 设置用户存档路径
    void setUserSavePath(const QString &path) { m_userSavePath = path; }

    // ==================== 配置管理 ====================

    // 从 UserData/path.json 加载配置
    bool load();

    // 保存配置到 UserData/path.json
    bool save() const;

    // 检查配置是否存在
    static bool configExists();

    // 检查配置是否有效（所有路径都已设置）
    bool isValid() const;

    // 获取默认用户存档路径（自动检测）
    static QString getDefaultUserSavePath();

    // 获取配置文件路径
    static QString getConfigFilePath();

private:
    QString m_gameInstallPath; // 游戏安装路径（例如：C:/Program Files (x86)/Steam/steamapps/common/RimWorld）
    QString m_steamPath;       // Steam 安装路径（例如：C:/Program Files (x86)/Steam）
    QString m_userSavePath;    // 用户存档路径（例如：C:/Users/xxx/AppData/LocalLow/Ludeon Studios/RimWorld by Ludeon Studios）
};

#endif // PATHCONFIG_H
