# ModConfigManager API 文档

## 概述

`ModConfigManager` 负责读取和写入 RimWorld 的 `ModsConfig.xml` 配置文件。该文件存储了游戏当前加载的 Mod 列表和顺序。

**类定义**: `src/data/ModConfigManager.h`

**配置文件位置**: 
```
C:\Users\{username}\AppData\LocalLow\Ludeon Studios\RimWorld by Ludeon Studios\Config\ModsConfig.xml
```

## 核心概念

### activeMods
包含**所有**激活的 Mod，包括：
- Core (`Ludeon.RimWorld`)
- 官方 DLC
- 创意工坊 Mod

### knownExpansions
**仅包含**官方 DLC 的 PackageId，**不包括** Core 和创意工坊 Mod。

这个字段由 `setActiveModsFromList()` 自动生成。

## 头文件引用

```cpp
#include "data/ModConfigManager.h"
```

## 构造函数

### ModConfigManager()
```cpp
ModConfigManager()
```

创建配置管理器，使用默认配置文件路径。

### ModConfigManager(const QString &configPath)
```cpp
explicit ModConfigManager(const QString &configPath)
```

创建配置管理器并指定配置文件路径。

**参数**:
- `configPath`: 自定义配置文件路径

---

## 配置读取

### loadConfig()
```cpp
bool loadConfig()
bool loadConfig(const QString &configPath)
```

从配置文件读取 Mod 列表。

**返回值**: 成功返回 `true`，失败返回 `false`

**示例**:
```cpp
ModConfigManager config;
if (config.loadConfig()) {
    qDebug() << "当前加载的 Mod:" << config.getActiveMods().size();
} else {
    qDebug() << "读取失败";
}
```

### loadConfigWithEmptyMods()
```cpp
bool loadConfigWithEmptyMods()
bool loadConfigWithEmptyMods(const QString &configPath)
```

创建空白的 Mod 列表（保留版本和其他字段）。

**用途**: 创建新的 Mod 配置列表

**效果**:
- `activeMods` 清空
- `knownExpansions` 清空
- 保留 `version` 和其他字段

**示例**:
```cpp
ModConfigManager config;
config.loadConfigWithEmptyMods();

qDebug() << "版本:" << config.getVersion();
qDebug() << "Mod数:" << config.getActiveMods().size();  // 0
```

---

## 配置保存

### saveConfig()
```cpp
bool saveConfig()
bool saveConfig(const QString &configPath)
```

保存配置到文件。

**⚠️ 警告**: 默认会写入游戏的配置文件，影响游戏实际加载的 Mod。

**建议**: 测试时使用 `UserDataManager::saveModListToPath()` 保存到自定义位置。

**示例**:
```cpp
config.saveConfig("UserData/ModList/my_config.xml");
```

---

## 数据访问

### getActiveMods()
```cpp
QStringList getActiveMods() const
```

获取当前激活的所有 Mod（按加载顺序）。

**返回值**: PackageId 列表

**示例**:
```cpp
QStringList mods = config.getActiveMods();
for (const QString &modId : mods) {
    qDebug() << modId;
}
```

### getKnownExpansions()
```cpp
QStringList getKnownExpansions() const
```

获取已知的官方 DLC 列表。

**返回值**: 官方 DLC 的 PackageId 列表

### getVersion()
```cpp
QString getVersion() const
```

获取游戏版本。

**示例**: `"1.5.4104"`

---

## 数据修改

### setActiveMods()
```cpp
void setActiveMods(const QStringList &mods)
```

设置激活的 Mod 列表。

**参数**: PackageId 列表

**注意**: 不会自动更新 `knownExpansions`，建议使用 `setActiveModsFromList()`

### setActiveModsFromList()
```cpp
void setActiveModsFromList(const QList<ModItem *> &modList)
```

从 ModItem 列表设置配置（推荐）。

**特性**:
- 自动设置 `activeMods`
- 自动生成 `knownExpansions`（过滤出官方 DLC）

**示例**:
```cpp
ModManager manager;
manager.scanAll();

QList<ModItem *> selectedMods;
// ... 选择要加载的 Mod

ModConfigManager config;
config.setActiveModsFromList(selectedMods);

qDebug() << "activeMods:" << config.getActiveMods().size();
qDebug() << "knownExpansions:" << config.getKnownExpansions().size();
```

---

## Mod 列表操作

### addMod()
```cpp
void addMod(const QString &modId)
```

添加 Mod 到列表末尾。

### insertMod()
```cpp
void insertMod(int index, const QString &modId)
```

在指定位置插入 Mod。

### removeMod()
```cpp
void removeMod(const QString &modId)
```

从列表中移除 Mod。

### moveModUp()
```cpp
bool moveModUp(const QString &modId)
```

将 Mod 向上移动一位。

**返回值**: 成功返回 `true`

### moveModDown()
```cpp
bool moveModDown(const QString &modId)
```

将 Mod 向下移动一位。

### moveModToPosition()
```cpp
bool moveModToPosition(const QString &modId, int newPosition)
```

将 Mod 移动到指定位置。

**示例**:
```cpp
config.moveModToPosition("brrainz.harmony", 1);  // 移到第二位
```

---

## 查询方法

### isModActive()
```cpp
bool isModActive(const QString &modId) const
```

检查 Mod 是否在激活列表中。

### getModPosition()
```cpp
int getModPosition(const QString &modId) const
```

获取 Mod 在列表中的位置。

**返回值**: 位置索引，未找到返回 `-1`

---

## 静态方法

### getDefaultConfigPath()
```cpp
static QString getDefaultConfigPath()
```

获取默认配置文件路径。

**返回值**: 游戏配置文件的完整路径

---

## 完整示例

### 示例 1: 读取和显示配置

```cpp
#include "data/ModConfigManager.h"

ModConfigManager config;
if (config.loadConfig()) {
    qDebug() << "游戏版本:" << config.getVersion();
    qDebug() << "\n激活的 Mod:";
    
    QStringList mods = config.getActiveMods();
    for (int i = 0; i < mods.size(); ++i) {
        qDebug() << QString("[%1] %2").arg(i).arg(mods[i]);
    }
    
    qDebug() << "\n官方 DLC:";
    for (const QString &dlc : config.getKnownExpansions()) {
        qDebug() << " -" << dlc;
    }
}
```

### 示例 2: 创建自定义 Mod 列表

```cpp
ModManager manager;
manager.setSteamPath(WorkshopScanner::detectSteamPath());
manager.scanAll();

// 构建 Mod 列表
QList<ModItem *> myMods;

// 添加 Core
ModItem *core = manager.findModByPackageId("Ludeon.RimWorld");
if (core) myMods.append(core);

// 添加 Harmony
ModItem *harmony = manager.findModByPackageId("brrainz.harmony");
if (harmony) myMods.append(harmony);

// 添加所有 DLC
myMods.append(manager.getOfficialDLCs());

// 应用配置
ModConfigManager config;
config.loadConfigWithEmptyMods();
config.setActiveModsFromList(myMods);

// 保存到自定义位置
config.saveConfig("UserData/ModList/my_config.xml");
```

### 示例 3: 调整加载顺序

```cpp
ModConfigManager config;
config.loadConfig();

// 确保 Harmony 在第二位（Core 之后）
if (config.isModActive("brrainz.harmony")) {
    config.moveModToPosition("brrainz.harmony", 1);
}

// 将某个 Mod 移到末尾
config.removeMod("some.mod.id");
config.addMod("some.mod.id");

config.saveConfig();
```

---

## 注意事项

1. **字段保留**: ModConfigManager 会保留配置文件中的所有未知字段
2. **knownExpansions 自动生成**: 使用 `setActiveModsFromList()` 会自动生成正确的 `knownExpansions`
3. **Core 不在 knownExpansions 中**: Core 是游戏核心，不是扩展包
4. **保存前备份**: 建议先备份原配置文件再保存

---

## 相关类

- [ModManager](ModManager_API.md)
- [ModItem](ModItem_API.md)
- [UserDataManager](UserDataManager_API.md)
