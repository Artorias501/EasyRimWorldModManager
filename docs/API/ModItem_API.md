# ModItem API 文档

## 概述

`ModItem` 是存储单个 Mod 所有信息的数据结构（struct）。它包含了 Mod 的基本信息、依赖关系、加载顺序等。

**类定义**: `src/data/ModItem.h`

## 结构定义

```cpp
struct ModItem
{
    // 基本信息
    QString identifier;            // 唯一标识符（PackageId）
    QString name;                  // Mod名称
    QString description;           // Mod描述
    QString author;                // 作者名
    QString url;                   // Mod链接
    QString packageId;             // 包ID（与identifier相同）
    QStringList supportedVersions; // 支持的游戏版本
    
    // 依赖和兼容性
    QStringList dependencies;      // 依赖的Mod列表
    QStringList loadBefore;        // 需要在这些Mod之前加载
    QStringList loadAfter;         // 需要在这些Mod之后加载
    QStringList incompatibleWith;  // 不兼容的Mod列表
    
    // 用户数据
    QString remark;                // 备注
    QString type;                  // Mod类型
    
    // 元数据
    bool isOfficialDLC = false;    // 是否为官方DLC
    QString sourcePath;            // Mod来源路径
    
    // 辅助方法
    void addDependency(const QString &dependency);
    void addLoadBefore(const QString &modId);
    void addLoadAfter(const QString &modId);
    void addIncompatibleWith(const QString &modId);
    void addSupportedVersion(const QString &version);
    bool isValid() const;
};
```

---

## 字段说明

### 基本信息字段

#### identifier
```cpp
QString identifier;
```

Mod 的唯一标识符，通常与 `packageId` 相同。

**来源**: About.xml 的 `<packageId>` 标签

**示例**: 
- `"ludeon.rimworld.royalty"`
- `"brrainz.harmony"`

---

#### name
```cpp
QString name;
```

Mod 的显示名称。

**来源**: About.xml 的 `<name>` 标签

**示例**: 
- `"Royalty"`
- `"Harmony"`
- `"RimWorld Core"` (Core 的默认名称)

**注意**: 
- 官方 DLC 可能没有 `<name>` 标签，会使用 packageId 作为名称
- Core 如果缺少 name，会使用 `"RimWorld Core"` 作为默认值

---

#### description
```cpp
QString description;
```

Mod 的描述文本。

**来源**: About.xml 的 `<description>` 标签

---

#### author
```cpp
QString author;
```

Mod 作者名称。

**来源**: About.xml 的 `<author>` 标签

**示例**: 
- `"Ludeon Studios"`
- `"Andreas Pardeike"`

---

#### url
```cpp
QString url;
```

Mod 的链接地址（通常是 Steam 创意工坊链接）。

**来源**: About.xml 的 `<url>` 标签

**示例**: 
- `"https://steamcommunity.com/sharedfiles/filedetails/?id=1234567890"`

---

#### packageId
```cpp
QString packageId;
```

包标识符，与 `identifier` 相同，保留用于未来扩展。

**用途**: 
- 作为 Mod 的唯一键
- 在 ModsConfig.xml 中标识 Mod
- 查找和索引 Mod

---

#### supportedVersions
```cpp
QStringList supportedVersions;
```

Mod 支持的游戏版本列表。

**来源**: About.xml 的 `<supportedVersions>` 或 `<targetVersion>` 标签

**示例**: 
```cpp
supportedVersions = {"1.5", "1.4"};
```

---

### 依赖和兼容性字段

#### dependencies
```cpp
QStringList dependencies;
```

Mod 依赖的其他 Mod 的 PackageId 列表。

**来源**: About.xml 的 `<modDependencies>` 标签

**示例**: 
```cpp
dependencies = {"brrainz.harmony", "unlimitedhugs.hugslib"};
```

**用途**: 
- 检查依赖是否满足
- 自动排序加载顺序
- 显示依赖警告

---

#### loadBefore
```cpp
QStringList loadBefore;
```

需要在这些 Mod 之前加载的 PackageId 列表。

**来源**: About.xml 的 `<loadBefore>` 标签

**含义**: 当前 Mod 应该在列表中的 Mod **之前** 加载

**示例**: 
```cpp
// Harmony 应该在大多数 Mod 之前加载
loadBefore = {"*"};  // "*" 表示所有其他 Mod
```

---

#### loadAfter
```cpp
QStringList loadAfter;
```

需要在这些 Mod 之后加载的 PackageId 列表。

**来源**: About.xml 的 `<loadAfter>` 标签

**含义**: 当前 Mod 应该在列表中的 Mod **之后** 加载

**示例**: 
```cpp
// 大多数 Mod 应该在 Core 之后加载
loadAfter = {"ludeon.rimworld"};
```

---

#### incompatibleWith
```cpp
QStringList incompatibleWith;
```

与当前 Mod 不兼容的 PackageId 列表。

**来源**: About.xml 的 `<incompatibleWith>` 标签

**用途**: 
- 检测冲突的 Mod
- 显示兼容性警告

---

### 用户数据字段

#### remark
```cpp
QString remark;
```

用户自定义的备注。

**来源**: UserDataManager (UserData/ModData/mod_data.json)

**用途**: 
- 用户为 Mod 添加个人笔记
- 标记重要信息
- 记录配置细节

**示例**: 
```cpp
mod->remark = "性能优化必备，必须放在前面";
```

---

#### type
```cpp
QString type;
```

Mod 的类型/分类。

**来源**: 
- 官方 DLC: 自动设置为 `"DLC"`
- Core: 自动设置为 `"Core"`
- 其他: 从 UserDataManager 加载或用户设置

**常见类型**: 
- `"前置框架"`
- `"功能性"`
- `"美化"`
- `"装备"`
- `"界面增强"`
- 等等（可自定义）

**示例**: 
```cpp
mod->type = "前置框架";
```

---

### 元数据字段

#### isOfficialDLC
```cpp
bool isOfficialDLC = false;
```

标识是否为官方 DLC。

**值**: 
- `true`: 官方 DLC（如 Royalty, Ideology, Biotech）
- `false`: 创意工坊 Mod 或 Core

**注意**: Core 的 `isOfficialDLC` 为 `false`

**判断方法**:
```cpp
if (mod->isOfficialDLC) {
    qDebug() << "这是官方 DLC";
} else if (mod->type == "Core") {
    qDebug() << "这是游戏核心";
} else {
    qDebug() << "这是创意工坊 Mod";
}
```

---

#### sourcePath
```cpp
QString sourcePath;
```

Mod 的来源路径。

**示例**: 
- 创意工坊: `"C:/Program Files (x86)/Steam/steamapps/workshop/content/294100/1234567890"`
- 官方 DLC: `"C:/Program Files (x86)/Steam/steamapps/common/RimWorld/Data/Royalty"`
- Core: `"C:/Program Files (x86)/Steam/steamapps/common/RimWorld/Data/Core"`

**用途**: 
- 定位 Mod 文件
- 区分 Mod 来源
- 调试和日志记录

---

## 辅助方法

### addDependency()
```cpp
void addDependency(const QString &dependency)
```

添加一个依赖项。

**参数**: 
- `dependency`: 依赖的 Mod 的 PackageId

**示例**: 
```cpp
mod->addDependency("brrainz.harmony");
mod->addDependency("unlimitedhugs.hugslib");
```

---

### addLoadBefore()
```cpp
void addLoadBefore(const QString &modId)
```

添加一个"在其之前加载"的 Mod。

**参数**: 
- `modId`: 需要在其之前加载的 Mod 的 PackageId

**示例**: 
```cpp
// Harmony 应该在所有 Mod 之前
harmonyMod->addLoadBefore("*");
```

---

### addLoadAfter()
```cpp
void addLoadAfter(const QString &modId)
```

添加一个"在其之后加载"的 Mod。

**参数**: 
- `modId`: 需要在其之后加载的 Mod 的 PackageId

**示例**: 
```cpp
// 大多数 Mod 应该在 Core 之后
myMod->addLoadAfter("ludeon.rimworld");
```

---

### addIncompatibleWith()
```cpp
void addIncompatibleWith(const QString &modId)
```

添加一个不兼容的 Mod。

**参数**: 
- `modId`: 不兼容的 Mod 的 PackageId

**示例**: 
```cpp
mod->addIncompatibleWith("conflicting.mod.packageid");
```

---

### addSupportedVersion()
```cpp
void addSupportedVersion(const QString &version)
```

添加一个支持的游戏版本。

**参数**: 
- `version`: 版本号字符串

**示例**: 
```cpp
mod->addSupportedVersion("1.5");
mod->addSupportedVersion("1.4");
```

---

### isValid()
```cpp
bool isValid() const
```

检查 ModItem 是否有效。

**返回值**: 
- `true`: ModItem 包含必要的信息
- `false`: ModItem 无效（通常是 packageId 为空）

**示例**: 
```cpp
if (mod->isValid()) {
    qDebug() << "有效的 Mod:" << mod->name;
} else {
    qDebug() << "无效的 Mod";
}
```

---

## 使用示例

### 示例 1: 读取 Mod 信息

```cpp
#include "data/ModManager.h"

void printModInfo(ModItem *mod) {
    if (!mod || !mod->isValid()) {
        qDebug() << "无效的 Mod";
        return;
    }
    
    qDebug() << "=== Mod 信息 ===";
    qDebug() << "名称:" << mod->name;
    qDebug() << "PackageId:" << mod->packageId;
    qDebug() << "作者:" << mod->author;
    qDebug() << "描述:" << mod->description;
    qDebug() << "类型:" << mod->type;
    qDebug() << "备注:" << mod->remark;
    qDebug() << "是否官方DLC:" << (mod->isOfficialDLC ? "是" : "否");
    
    qDebug() << "\n支持的版本:";
    for (const QString &version : mod->supportedVersions) {
        qDebug() << "  -" << version;
    }
    
    qDebug() << "\n依赖项:";
    for (const QString &dep : mod->dependencies) {
        qDebug() << "  -" << dep;
    }
    
    if (!mod->loadAfter.isEmpty()) {
        qDebug() << "\n需要在这些Mod之后加载:";
        for (const QString &after : mod->loadAfter) {
            qDebug() << "  -" << after;
        }
    }
}

// 使用
ModManager manager;
manager.setSteamPath(WorkshopScanner::detectSteamPath());
manager.scanAll();

ModItem *harmony = manager.findModByPackageId("brrainz.harmony");
printModInfo(harmony);
```

### 示例 2: 设置用户数据

```cpp
void setModUserData(ModManager &manager, const QString &packageId) {
    ModItem *mod = manager.findModByPackageId(packageId);
    if (!mod) {
        qDebug() << "Mod 未找到";
        return;
    }
    
    // 设置类型
    if (mod->name.contains("Harmony", Qt::CaseInsensitive)) {
        mod->type = "前置框架";
    } else if (mod->name.contains("UI", Qt::CaseInsensitive)) {
        mod->type = "界面增强";
    } else {
        mod->type = "功能性";
    }
    
    // 设置备注
    if (!mod->dependencies.isEmpty()) {
        mod->remark = QString("依赖 %1 个其他 Mod").arg(mod->dependencies.size());
    } else {
        mod->remark = "无依赖，可以独立运行";
    }
    
    // 保存
    manager.saveModsToUserData();
    
    qDebug() << "已为" << mod->name << "设置用户数据";
}
```

### 示例 3: 检查依赖

```cpp
bool checkDependencies(ModManager &manager, ModItem *mod) {
    if (!mod || mod->dependencies.isEmpty()) {
        return true;  // 无依赖
    }
    
    qDebug() << "检查" << mod->name << "的依赖...";
    
    bool allSatisfied = true;
    for (const QString &depId : mod->dependencies) {
        ModItem *depMod = manager.findModByPackageId(depId);
        if (!depMod) {
            qDebug() << "  ❌ 缺失依赖:" << depId;
            allSatisfied = false;
        } else {
            qDebug() << "  ✅ 已安装:" << depMod->name;
        }
    }
    
    return allSatisfied;
}

// 使用
ModManager manager;
manager.scanAll();

for (ModItem *mod : manager.getAllMods()) {
    if (!checkDependencies(manager, mod)) {
        qDebug() << "\n警告:" << mod->name << "的依赖不完整\n";
    }
}
```

### 示例 4: 过滤和分类

```cpp
#include <QMap>

QMap<QString, QList<ModItem*>> categorizeModsByType(ModManager &manager) {
    QMap<QString, QList<ModItem*>> categories;
    
    for (ModItem *mod : manager.getAllMods()) {
        QString category = mod->type.isEmpty() ? "未分类" : mod->type;
        categories[category].append(mod);
    }
    
    return categories;
}

// 使用
ModManager manager;
manager.scanAll();

QMap<QString, QList<ModItem*>> categories = categorizeModsByType(manager);

for (auto it = categories.begin(); it != categories.end(); ++it) {
    qDebug() << "\n=== " << it.key() << " ===";
    qDebug() << "数量:" << it.value().size();
    
    for (ModItem *mod : it.value()) {
        qDebug() << "  -" << mod->name;
    }
}
```

### 示例 5: 构建加载顺序

```cpp
QList<ModItem*> buildLoadOrder(ModManager &manager) {
    QList<ModItem*> ordered;
    QList<ModItem*> allMods = manager.getAllMods();
    
    // 1. 首先添加 Core
    for (ModItem *mod : allMods) {
        if (mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0) {
            ordered.append(mod);
            break;
        }
    }
    
    // 2. 添加前置框架（Harmony, HugsLib 等）
    for (ModItem *mod : allMods) {
        if (mod->type == "前置框架") {
            ordered.append(mod);
        }
    }
    
    // 3. 添加官方 DLC
    ordered.append(manager.getOfficialDLCs());
    
    // 4. 添加其他工坊 Mod
    for (ModItem *mod : manager.getWorkshopMods()) {
        if (mod->type != "前置框架") {
            ordered.append(mod);
        }
    }
    
    return ordered;
}

// 使用
ModManager manager;
manager.scanAll();

QList<ModItem*> orderedMods = buildLoadOrder(manager);

qDebug() << "推荐的加载顺序:";
for (int i = 0; i < orderedMods.size(); ++i) {
    qDebug() << QString("[%1] %2").arg(i).arg(orderedMods[i]->name);
}
```

---

## 数据来源

### 从 About.xml 解析

ModItem 的大部分字段来自 Mod 目录下的 `About/About.xml` 文件：

```xml
<?xml version="1.0" encoding="utf-8"?>
<ModMetaData>
    <name>Harmony</name>
    <packageId>brrainz.harmony</packageId>
    <author>Andreas Pardeike</author>
    <description>A RimWorld Mod Harmony library</description>
    <url>https://github.com/pardeike/HarmonyRimWorld</url>
    
    <supportedVersions>
        <li>1.5</li>
        <li>1.4</li>
    </supportedVersions>
    
    <modDependencies />
    
    <loadBefore>
        <li>*</li>
    </loadBefore>
</ModMetaData>
```

### 从 UserDataManager 加载

用户数据存储在 `UserData/ModData/mod_data.json`:

```json
{
    "brrainz.harmony": {
        "type": "前置框架",
        "remark": "必须最先加载"
    },
    "ludeon.rimworld.royalty": {
        "type": "DLC",
        "remark": "官方扩展包"
    }
}
```

---

## 注意事项

### 1. 内存管理

- ❌ **不要** 手动 `delete` ModItem 指针
- ✅ ModItem 由扫描器（WorkshopScanner/OfficialDLCScanner）创建和管理
- ✅ ModManager 清理时会自动删除所有 ModItem

### 2. 字符串比较

- PackageId 比较应该**不区分大小写**
- 使用 `QString::compare(other, Qt::CaseInsensitive)`

```cpp
// 正确
if (mod->packageId.compare("Ludeon.RimWorld", Qt::CaseInsensitive) == 0) {
    // ...
}

// 错误
if (mod->packageId == "Ludeon.RimWorld") {  // 大小写敏感
    // ...
}
```

### 3. 空值处理

- 某些字段可能为空（description, author, url 等）
- 使用前应检查 `isEmpty()`

```cpp
if (!mod->description.isEmpty()) {
    qDebug() << mod->description;
}
```

### 4. Core 的特殊性

- Core 的 PackageId 是 `"Ludeon.RimWorld"` (不含 "core")
- Core 的 `isOfficialDLC` 为 `false`
- Core 的 `type` 为 `"Core"`

---

## 相关类

- [ModManager](ModManager_API.md) - 管理所有 ModItem
- [WorkshopScanner](WorkshopScanner_API.md) - 创建创意工坊 ModItem
- [OfficialDLCScanner](OfficialDLCScanner_API.md) - 创建官方内容 ModItem
- [UserDataManager](UserDataManager_API.md) - 管理 ModItem 的用户数据

---

## 版本历史

- **v1.0** (2025-11-22): 初始版本
  - 基本字段定义
  - 依赖关系支持
  - 用户数据字段
