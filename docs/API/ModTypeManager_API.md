# ModTypeManager API 文档

## 概述

`ModTypeManager` 管理 Mod 的类型分类系统，包括默认类型和用户自定义类型。

**类定义**: `src/data/ModTypeManager.h`

## 默认类型

系统预定义的 Mod 类型：

- `"Core"` - 游戏核心
- `"DLC"` - 官方 DLC
- `"前置框架"` - 如 Harmony、HugsLib
- `"功能性"` - 功能性 Mod
- `"美化"` - 美化类 Mod
- `"装备"` - 武器、护甲等
- `"界面增强"` - UI 相关
- `"其他"` - 未分类

## 头文件引用

```cpp
#include "data/ModTypeManager.h"
```

## 基本操作

### getAllTypes()
```cpp
QStringList getAllTypes() const
```

获取所有可用的类型（默认 + 自定义）。

### addCustomType()
```cpp
bool addCustomType(const QString &type)
```

添加自定义类型。

**示例**:
```cpp
ModTypeManager typeMgr;
typeMgr.addCustomType("性能优化");
typeMgr.addCustomType("剧情增强");
```

### removeCustomType()
```cpp
bool removeCustomType(const QString &type)
```

移除自定义类型。

**注意**: 不能移除默认类型。

### hasType()
```cpp
bool hasType(const QString &type) const
```

检查类型是否存在。

### isDefaultType()
```cpp
bool isDefaultType(const QString &type) const
```

检查是否为默认类型。

---

## 静态方法

### getDefaultTypes()
```cpp
static QStringList getDefaultTypes()
```

获取默认类型列表。

---

## 使用示例

```cpp
ModTypeManager typeMgr;

// 获取所有类型
QStringList allTypes = typeMgr.getAllTypes();
for (const QString &type : allTypes) {
    qDebug() << type;
}

// 添加自定义类型
typeMgr.addCustomType("我的自定义类型");

// 检查类型
if (typeMgr.hasType("功能性")) {
    qDebug() << "类型存在";
}
```

---

## 相关类

- [UserDataManager](UserDataManager_API.md) - 也管理自定义类型
- [ModItem](ModItem_API.md) - 使用类型字段
