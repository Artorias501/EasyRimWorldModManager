# UserDataManager API 文档

## 概述

`UserDataManager` 管理用户的持久化数据，包括 Mod 的备注、类型分类和自定义类型列表。数据存储在程序目录的 `UserData/` 文件夹中。

**类定义**: `src/data/UserDataManager.h`

**重要更新**: 从 v1.1 开始，UserDataManager 整合了原 ModTypeManager 的功能，统一管理默认类型和自定义类型。

## 数据存储位置

```
UserData/
├── ModData/
│   ├── mod_data.json         # Mod 备注和类型
│   └── custom_types.json     # 自定义类型列表
└── ModList/
    └── *.xml                  # 保存的 Mod 列表
```

## 头文件引用

```cpp
#include "data/UserDataManager.h"
```

## Mod 类型和备注管理

### getModType()
```cpp
QString getModType(const QString &packageId) const
```

获取 Mod 的类型。

**返回值**: 类型字符串，未设置返回空字符串

### setModType()
```cpp
void setModType(const QString &packageId, const QString &type)
```

设置 Mod 的类型。

**示例**:
```cpp
UserDataManager userMgr;
userMgr.setModType("brrainz.harmony", "前置框架");
userMgr.saveModData();
```

### getModRemark()
```cpp
QString getModRemark(const QString &packageId) const
```

获取 Mod 的备注。

### setModRemark()
```cpp
void setModRemark(const QString &packageId, const QString &remark)
```

设置 Mod 的备注。

---

## 类型管理（整合自 ModTypeManager）

### getAllTypes()
```cpp
QStringList getAllTypes() const
```

获取所有可用类型（默认类型 + 自定义类型）。

**返回值**: 完整的类型列表

**示例**:
```cpp
UserDataManager userMgr;
QStringList allTypes = userMgr.getAllTypes();
// 包含：核心、DLC、前置框架、逻辑mod、功能性mod... 以及用户自定义类型
```

---

### getDefaultTypes()
```cpp
static QStringList getDefaultTypes()
```

获取系统默认类型列表（静态方法）。

**默认类型**:
- 核心
- DLC
- 前置框架
- 逻辑mod
- 功能性mod
- 种族mod
- 种族扩展mod
- 单一功能mod
- 汉化
- 优化

**示例**:
```cpp
QStringList defaultTypes = UserDataManager::getDefaultTypes();
```

---

### getAllCustomTypes()
```cpp
QStringList getAllCustomTypes() const
```

获取所有自定义类型（不包括默认类型）。

---

### addCustomType()
```cpp
bool addCustomType(const QString &type)
```

添加自定义类型。

**返回值**: 
- `true`: 添加成功
- `false`: 失败（空字符串、与默认类型重复、已存在）

**示例**:
```cpp
if (userMgr.addCustomType("性能优化")) {
    qDebug() << "类型添加成功";
    userMgr.saveCustomTypes();
}
```

---

### removeCustomType()
```cpp
bool removeCustomType(const QString &type)
```

移除自定义类型。

**返回值**: 
- `true`: 删除成功
- `false`: 失败（是默认类型或不存在）

**注意**: 不能删除默认类型。

---

### hasType()
```cpp
bool hasType(const QString &type) const
```

检查类型是否存在（包括默认类型和自定义类型）。

**示例**:
```cpp
if (userMgr.hasType("前置框架")) {
    qDebug() << "类型存在";
}
```

---

### isDefaultType()
```cpp
bool isDefaultType(const QString &type) const
```

检查是否为默认类型。

---

### hasCustomType()
```cpp
bool hasCustomType(const QString &type) const
```

检查是否为自定义类型（不包括默认类型）。

---

## 数据持久化

### loadAll()
```cpp
bool loadAll()
```

加载所有用户数据（Mod 数据和自定义类型）。

### saveAll()
```cpp
bool saveAll()
```

保存所有用户数据。

### loadModData()
```cpp
bool loadModData()
```

加载 Mod 数据（类型和备注）。

### saveModData()
```cpp
bool saveModData()
```

保存 Mod 数据。

### loadCustomTypes()
```cpp
bool loadCustomTypes()
```

加载自定义类型列表。

### saveCustomTypes()
```cpp
bool saveCustomTypes()
```

保存自定义类型列表。

---

## Mod 列表管理

### saveModListToPath()
```cpp
bool saveModListToPath(const QString &filePath,
                       const QStringList &activeMods,
                       const QStringList &knownExpansions,
                       const QString &version,
                       const QMap<QString, QString> &otherData = {})
```

保存 Mod 列表到指定文件。

**示例**:
```cpp
UserDataManager userMgr;
userMgr.saveModListToPath(
    "UserData/ModList/backup.xml",
    config.getActiveMods(),
    config.getKnownExpansions(),
    config.getVersion()
);
```

---

## 静态方法

### getUserDataPath()
```cpp
static QString getUserDataPath()
```

获取用户数据根目录。

### getModDataPath()
```cpp
static QString getModDataPath()
```

获取 Mod 数据目录。

### getModListPath()
```cpp
static QString getModListPath()
```

获取 Mod 列表目录。

### initializeDirectories()
```cpp
static bool initializeDirectories()
```

初始化目录结构。

---

## 使用示例

### 示例 1: 管理 Mod 类型

```cpp
#include "data/UserDataManager.h"

UserDataManager userMgr;
userMgr.loadAll();

// 获取所有可用类型
QStringList allTypes = userMgr.getAllTypes();
qDebug() << "可用类型:" << allTypes;

// 添加自定义类型
if (userMgr.addCustomType("我的自定义类型")) {
    userMgr.saveCustomTypes();
}

// 为 Mod 设置类型
userMgr.setModType("brrainz.harmony", "前置框架");
userMgr.setModRemark("brrainz.harmony", "必须最先加载");
userMgr.saveModData();
```

### 示例 2: 在 UI 中使用类型列表

```cpp
// 填充类型下拉框
void loadTypeComboBox(QComboBox *comboBox, UserDataManager *userMgr) {
    comboBox->clear();
    comboBox->addItem("未分类", "");
    
    QStringList types = userMgr->getAllTypes();
    for (const QString &type : types) {
        comboBox->addItem(type, type);
    }
}
```

### 示例 3: 类型验证

```cpp
void setModType(UserDataManager *userMgr, const QString &packageId, const QString &type) {
    if (!userMgr->hasType(type)) {
        qDebug() << "类型不存在，是否要添加？";
        if (userMgr->addCustomType(type)) {
            userMgr->saveCustomTypes();
        }
    }
    
    userMgr->setModType(packageId, type);
    userMgr->saveModData();
}
```

---

## 版本历史

- **v1.1** (2025-11-22): 整合 ModTypeManager 功能
  - 添加 `getAllTypes()` - 获取所有类型
  - 添加 `getDefaultTypes()` - 获取默认类型
  - 添加 `hasType()` - 检查类型是否存在
  - 添加 `isDefaultType()` - 检查是否为默认类型
  - 增强 `addCustomType()` - 防止与默认类型重复
  - 增强 `removeCustomType()` - 防止删除默认类型
  - **废弃**: ModTypeManager 类已被移除

- **v1.0** (2025-11-22): 初始版本
  - 基本的 Mod 数据管理
  - 自定义类型管理
  - 数据持久化

---

## 相关类

- [ModManager](ModManager_API.md)
- [ModConfigManager](ModConfigManager_API.md)
- [ModItem](ModItem_API.md)

---

## 迁移指南

如果你之前使用了 `ModTypeManager`，请按以下方式迁移：

### 之前的代码：
```cpp
ModTypeManager typeManager;
QStringList types = typeManager.getAllTypes();
typeManager.addCustomType("新类型");
```

### 迁移后的代码：
```cpp
UserDataManager userMgr;
QStringList types = userMgr.getAllTypes();
userMgr.addCustomType("新类型");
```

**主要变化**:
1. 使用 `UserDataManager` 替代 `ModTypeManager`
2. 所有方法名称保持不变
3. `getDefaultTypes()` 现在是静态方法
4. 数据自动持久化到同一位置
