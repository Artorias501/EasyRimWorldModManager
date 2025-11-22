# UserDataManager API 文档

## 概述

`UserDataManager` 管理用户的持久化数据，包括 Mod 的备注、类型分类和自定义类型列表。数据存储在程序目录的 `UserData/` 文件夹中。

**类定义**: `src/data/UserDataManager.h`

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

## 自定义类型管理

### getAllCustomTypes()
```cpp
QStringList getAllCustomTypes() const
```

获取所有自定义类型。

### addCustomType()
```cpp
bool addCustomType(const QString &type)
```

添加自定义类型。

### removeCustomType()
```cpp
bool removeCustomType(const QString &type)
```

移除自定义类型。

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

### initializeDirectories()
```cpp
static bool initializeDirectories()
```

初始化目录结构。

---

## 相关类

- [ModManager](ModManager_API.md)
- [ModConfigManager](ModConfigManager_API.md)
