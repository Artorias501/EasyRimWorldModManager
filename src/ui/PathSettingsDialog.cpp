#include "PathSettingsDialog.h"
#include "../data/WorkshopScanner.h"
#include "ui_PathSettingsDialog.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

PathSettingsDialog::PathSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PathSettingsDialog), m_pathConfig(nullptr)
{
    ui->setupUi(this);
    setupConnections();
    loadDefaultPaths();
}

PathSettingsDialog::PathSettingsDialog(PathConfig *config, QWidget *parent)
    : QDialog(parent), ui(new Ui::PathSettingsDialog), m_pathConfig(config)
{
    ui->setupUi(this);
    setupConnections();

    if (m_pathConfig)
    {
        setPathConfig(*m_pathConfig);
    }
    else
    {
        loadDefaultPaths();
    }
}

PathSettingsDialog::~PathSettingsDialog()
{
    delete ui;
}

void PathSettingsDialog::setupConnections()
{
    connect(ui->browseSteamButton, &QPushButton::clicked, this, &PathSettingsDialog::onBrowseSteamPath);
    connect(ui->browseGameButton, &QPushButton::clicked, this, &PathSettingsDialog::onBrowseGamePath);
    connect(ui->browseSaveButton, &QPushButton::clicked, this, &PathSettingsDialog::onBrowseSavePath);
}

void PathSettingsDialog::loadDefaultPaths()
{
    // 尝试自动检测 Steam 路径
    QString detectedSteamPath = WorkshopScanner::detectSteamPath();
    if (!detectedSteamPath.isEmpty())
    {
        ui->steamPathEdit->setText(detectedSteamPath);

        // 基于 Steam 路径推断游戏路径
        QString defaultGamePath = QDir(detectedSteamPath).absoluteFilePath("steamapps/common/RimWorld");
        if (QDir(defaultGamePath).exists())
        {
            ui->gamePathEdit->setText(defaultGamePath);
        }
    }

    // 设置默认用户存档路径
    QString defaultSavePath = PathConfig::getDefaultUserSavePath();
    ui->savePathEdit->setText(defaultSavePath);
}

PathConfig PathSettingsDialog::getPathConfig() const
{
    PathConfig config;
    config.setSteamPath(ui->steamPathEdit->text());
    config.setGameInstallPath(ui->gamePathEdit->text());
    config.setUserSavePath(ui->savePathEdit->text());
    return config;
}

void PathSettingsDialog::setPathConfig(const PathConfig &config)
{
    ui->steamPathEdit->setText(config.getSteamPath());
    ui->gamePathEdit->setText(config.getGameInstallPath());
    ui->savePathEdit->setText(config.getUserSavePath());
}

void PathSettingsDialog::accept()
{
    if (validatePaths())
    {
        QDialog::accept();
    }
}

bool PathSettingsDialog::validatePaths()
{
    QString steamPath = ui->steamPathEdit->text();
    QString gamePath = ui->gamePathEdit->text();
    QString savePath = ui->savePathEdit->text();

    // 检查是否为空
    if (steamPath.isEmpty())
    {
        QMessageBox::warning(this, "路径验证", "Steam 安装路径不能为空");
        ui->steamPathEdit->setFocus();
        return false;
    }

    if (gamePath.isEmpty())
    {
        QMessageBox::warning(this, "路径验证", "游戏安装路径不能为空");
        ui->gamePathEdit->setFocus();
        return false;
    }

    if (savePath.isEmpty())
    {
        QMessageBox::warning(this, "路径验证", "用户存档路径不能为空");
        ui->savePathEdit->setFocus();
        return false;
    }

    // 检查路径是否存在
    if (!QDir(steamPath).exists())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "路径验证",
            QString("Steam 安装路径不存在：\n%1\n\n是否继续？").arg(steamPath),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            ui->steamPathEdit->setFocus();
            return false;
        }
    }

    if (!QDir(gamePath).exists())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "路径验证",
            QString("游戏安装路径不存在：\n%1\n\n是否继续？").arg(gamePath),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            ui->gamePathEdit->setFocus();
            return false;
        }
    }

    if (!QDir(savePath).exists())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "路径验证",
            QString("用户存档路径不存在：\n%1\n\n是否继续？").arg(savePath),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            ui->savePathEdit->setFocus();
            return false;
        }
    }

    return true;
}

void PathSettingsDialog::onBrowseSteamPath()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "选择 Steam 安装目录",
        ui->steamPathEdit->text().isEmpty() ? "C:/Program Files (x86)/Steam" : ui->steamPathEdit->text());

    if (!dir.isEmpty())
    {
        ui->steamPathEdit->setText(dir);

        // 自动推断游戏路径
        QString gamePath = QDir(dir).absoluteFilePath("steamapps/common/RimWorld");
        if (QDir(gamePath).exists() && ui->gamePathEdit->text().isEmpty())
        {
            ui->gamePathEdit->setText(gamePath);
        }
    }
}

void PathSettingsDialog::onBrowseGamePath()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "选择游戏安装目录",
        ui->gamePathEdit->text().isEmpty() ? "C:/Program Files (x86)/Steam/steamapps/common/RimWorld"
                                           : ui->gamePathEdit->text());

    if (!dir.isEmpty())
    {
        ui->gamePathEdit->setText(dir);
    }
}

void PathSettingsDialog::onBrowseSavePath()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        "选择用户存档目录",
        ui->savePathEdit->text().isEmpty() ? PathConfig::getDefaultUserSavePath() : ui->savePathEdit->text());

    if (!dir.isEmpty())
    {
        ui->savePathEdit->setText(dir);
    }
}
